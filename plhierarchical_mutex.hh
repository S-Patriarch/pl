//
// (c) 2024 S-Patriarch
// Patriarch library : plhierarchical_mutex.hh
//
#ifndef PL_PLHIERARCHICAL_MUTEX_HH
#define PL_PLHIERARCHICAL_MUTEX_HH

#include <limits.h>
#include <mutex>
#include <stdexcept>

namespace pl {
   class hierarchical_mutex {
   	// иерархия блокировок
      //	иерархия блокировок позволяет обеспечить средство проверки соблюдения
      // соглашения в ходе выполнения программы
      // замысел заключается в том, чтобы разбить приложение на уровни и
      // идентифицировать все мьютексы, которые могут быть заблокированы на 
      // любом заданном уровне
      // когда код пытается заблокировать мьютекс, он не может этого сделать, 
      // если в нем уже содержится блокировка из нижнего уровня
      // это можно проверить в ходе выполнения программы, назначив номера 
      // уровней каждому мьютексу и сохранив записи о том, какие мьютексы 
      // заблокированы каждым потоком
      // в данной реализации задействуется локальная по отношению к потоку 
      // переменная, предназначенная для хранения текущего значения уровня 
      // иерархии
      // данное значение доступно всем экземплярам мьютексов, но оно разное для
      // каждого из потоков
      // это позволяет коду проверять поведение каждого потока по отдельности, и
      // код для каждого мьютекса может проверять, разрешено текущему потоку
      // заблокировать этот мьютекс или нет
      // главное здесь заключается в использовании значения thread_local, 
      // представляющего собой значение уровня иерархии текущего потока 
      // m_this_thread_hierarchy_value
      // оно инициализируется максимальным значением, поэтому изначально
      // заблокирован может быть любой мьютекс
      // поскольку это значение объявлено как thread_local, собственная копия 
      // может быть у каждого потока, поэтому состояние переменной в одном 
      // потоке совершенно не зависит от состояния переменной, когда чтение 
      // происходит из другого потока
      //
      // компонентные функции:
      // lock()     - блокирует мьютекс
      // unlock()   - разблокирует мьютекс
      // try_lock() - пытается заблокировать мьютекс
   private:
   	std::mutex                         m_internal_mutex;
   	unsigned long const                m_hierarchy_value;
   	unsigned long                      m_previous_hierarchy_value;
   	static thread_local unsigned long  m_this_thread_hierarchy_value;
      void check_for_hierarchy_violation()
      {
      	if (m_this_thread_hierarchy_value<=m_hierarchy_value)
      		throw std::logic_error("E: Иерархия мьютексов нарушена.");
      }
      void update_hierarchy_value()
      {
         m_previous_hierarchy_value = m_this_thread_hierarchy_value;
         m_this_thread_hierarchy_value = m_hierarchy_value;
      }
   public:
   	explicit hierarchical_mutex(unsigned long value) :
   	   m_hierarchy_value{value},
   	   m_previous_hierarchy_value{0}
   	{}
   	void lock()
   	{
   		check_for_hierarchy_violation();
   		m_internal_mutex.lock();
   		update_hierarchy_value();
   	}
   	void unlock()
   	{
   		if (m_this_thread_hierarchy_value!=m_hierarchy_value)
   			throw std::logic_error("E: Иерархия мьютексов нарушена.");
   		m_this_thread_hierarchy_value = m_previous_hierarchy_value;
   		m_internal_mutex.unlock();
   	}
   	bool try_lock()
   	{
   		check_for_hierarchy_violation();
   		if (!m_internal_mutex.try_lock()) return false;
   		update_hierarchy_value();
   		return true;
   	}
   };
   thread_local unsigned long
   hierarchical_mutex::m_this_thread_hierarchy_value{ULONG_MAX};
}
#endif // PL_PLHIERARCHICAL_MUTEX_HH