//
// (c) 2024 S-Patriarch
// Patriarch library : logger.hh
//
#ifndef PL_LOGGER_HH
#define PL_LOGGER_HH

#ifndef PL_EXCEPT_HH
#include "except.hh"
#endif

#include <fstream>
#include <iostream>
#include <string>
#include <mutex>
#include <shared_mutex>

namespace pl {
   class Logger {
      // класс для обработки логов в многопоточной среде
      //
      // при создании обьекта, конструктор принимает путь и имя файла
      // в который будет вестись отписывание логов
      // при уничтожении обьекта данного класса, деструктор
      // закрывает открытый конструктором файловый поток
      //
      // компонентные функции:
      // write() - запись строки в журнал логов
      // read()  - чтение строки из журнала логов
   private:
      std::fstream      m_file;     // файловый поток
      std::shared_mutex m_mutex;    // мьютекс
   public:
      Logger(const std::string& s_flog)
      {
         m_file.open(s_flog, std::ios::in | std::ios::out | std::ios::app);
         if (!m_file.is_open()) 
            throw Exception("E: failed to open log file.");

         // установка флагов синхронизации для потока ввода-вывода
         // позволяющих управлять тем, как и когда данные
         // записываются на диск
         
         // флаг для отключения синхронизации потока с потоками ввода-вывода С
         m_file.sync_with_stdio(false);
         // флаг для немедленной записи данных в файл
         m_file.unitbuf();
      }
      ~Logger() noexcept
      {
         if (m_file.is_open()) m_file.close();
      }
      void write(const std::string& s)
         // запись строки в журнал логов
         // создание объекта lck типа std::unique_lock, который блокирует
         // мьютекс m_mutex в режиме монопольной (или уникальной) блокировки
         // это значит, что только один поток может иметь доступ к общему
         // ресурсу, защищенному m_mutex, в данный момент времени
         // объект lck будет автоматически освобожден при выходе из
         // области видимости
      {
         std::unique_lock<std::shared_mutex> lck(m_mutex);
         if (m_file.is_open()) m_file << s << '\n';
      }
      std::string read()
         // чтение строки из журнала логов
         // создание объекта lck типа std::shared_lock, который блокирует
         // мьютекс m_mutex в режиме совместной блокировки
         // это позволяет нескольким потокам иметь одновременный доступ к
         // общему ресурсу
         // объект lck будет автоматически освобожден при выходе из
         // области видимости
      {
         std::string s_res {};
         std::shared_lock<std::shared_mutex> lck(m_mutex);
         if (m_file.is_open()) std::getline(m_file, s_res);
         return s_res;
      }
   };
}
#endif // PL_LOGGER_HH
