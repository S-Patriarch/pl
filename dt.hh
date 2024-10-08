/*
 * Copyright (C) 2024, S-Patriarch
 * Patriarch library : dt.hh
 */

#ifndef PL_DT_HH
#define PL_DT_HH 1

#include <chrono>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace pl {
  class DateTime {
    /*
     * класс для работы с датой и временем
     *
     * компонентные функции:
     * get_date()        - получение текущей даты 
     * get_time()        - получение текущего времени
     * date_conversion() - преобразует строку даты в иной вид строки
     */
  public:
    enum class dcf {
      DCF_dBY = 0,
      DCF_BdY
    }; 

    std::string get_date() const
    {
      std::chrono::system_clock::time_point 
      today {std::chrono::system_clock::now()};
      std::time_t t {std::chrono::system_clock::to_time_t(today)};
      std::stringstream ss;
      ss << std::put_time(std::localtime(&t), "%d-%m-%Y");
      std::string res {ss.str()};
      return res;
    }

    std::string get_time() const
    {
      // получаем текущее время
      std::time_t now {std::time(nullptr)};
      // преобразуем текущее время в локальное время
      std::tm* lt {std::localtime(&now)};
      // форматируем локальное время в строку
      char buf[9];
      std::strftime(buf,sizeof(buf), "%H:%M:%S", lt);
      // преобразуем buf в std::string
      std::string res {buf};
      return res;
    }

    std::string date_conversion(const std::string& dateString, 
                                dcf dcf =dcf::DCF_dBY) const
    {
      std::tm tm;
      std::istringstream ss(dateString);
      ss >> std::get_time(&tm, "%d-%m-%Y");

      char bufFormattedDate[80] {""};
      if (dcf==dcf::DCF_dBY) 
        std::strftime(bufFormattedDate, sizeof(bufFormattedDate), "%d %B %Y", &tm);
      else if (dcf==dcf::DCF_BdY) 
        std::strftime(bufFormattedDate, sizeof(bufFormattedDate), "%B %d, %Y", &tm);

      return static_cast<std::string>(bufFormattedDate);
    }
  };
}

#endif
