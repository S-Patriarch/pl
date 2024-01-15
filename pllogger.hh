//
// (c) 2024 S-Patriarch
// Patriarch library : pllogger.hh
//
#ifndef PL_PLLOGGER_HH
#define PL_PLLOGGER_HH

#include <fstream>
#include <iostream>
#include <string>
#include <shared_mutex>

namespace pl {
   class Logger {
      // класс для обработки логов в многопоточной среде
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
            std::cerr << "\nE: Не удалось открыть файл журнала.\n";
      }
      ~Logger() noexcept
      {
         if (m_file.is_open()) m_file.close();
      }
      void write(const std::string& s)
         // запись строки в журнал логов
         // создание объекта lock типа std::unique_lock, который блокирует
         // мьютекс m_mutex в режиме монопольной (или уникальной) блокировки
         // это значит, что только один поток может иметь доступ к общему
         // ресурсу, защищенному m_mutex, в данный момент времени
         // объект lock будет автоматически освобожден при выходе из
         // области видимости
      {
         std::unique_lock<std::shared_mutex> lock(m_mutex);
         if (m_file.is_open()) m_file << s << '\n';
      }
      std::string read()
         // чтение строки из журнала логов
         // создание объекта lock типа std::shared_lock, который блокирует
         // мьютекс m_mutex в режиме совместной блокировки
         // это позволяет нескольким потокам иметь одновременный доступ к
         // общему ресурсу
         // объект lock будет автоматически освобожден при выходе из
         // области видимости
      {
         std::shared_lock<std::shared_mutex> lock(m_mutex);
         std::string s_res {};
         if (m_file.is_open()) std::getline(m_file, s_res);
         return s_res;
      }
   };
}
#endif // PL_PLLOGGER_HH
