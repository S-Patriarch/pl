//
// (c) 2024 S-Patriarch
// Patriarch library : dt.hh
//
#ifndef PL_DT_HH
#define PL_DT_HH

#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace pl {
   class Dt {
      // класс для работы с датой и временем
      //
      // компонентные функции:
      // get_date() - получение текущей даты 
      // get_time() - получение текущего времени
   public:
      std::string get_date()
         // получает текущую дату и возвращает ее в виде строки
      {
         std::chrono::system_clock::time_point 
         today {std::chrono::system_clock::now()};
         std::time_t t {std::chrono::system_clock::to_time_t(today)};
         std::stringstream ss;
         ss << std::put_time(std::localtime(&t),"%d-%m-%Y");
         std::string res {ss.str()};
         return res;
      }
      std::string get_time()
         // получает текущее время и возвращает его в виде строки
      {
         // получаем текущее время
         std::time_t now {std::time(nullptr)};
         // преобразуем текущее время в локальное время
         std::tm* local_time {std::localtime(&now)};
         // форматируем локальное время в строку
         char buffer[9];
         std::strftime(buffer,sizeof(buffer),"%H:%M:%S",local_time);
         // преобразуем buffer в std::string
         std::string res {buffer};
         return res;
      }
  };
}
#endif // PL_DT_HH
