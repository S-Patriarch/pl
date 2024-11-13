/*
 * Copyright (C) 2024, S-Patriarch
 * Patriarch library : string.hh
 */

#ifndef PL_STRING_HH
#define PL_STRING_HH 1

#include <string>
#include <vector>
#include <sstream>
#include <cctype>

namespace pl {
  class String {
  /*
   * класс для работы со строками
   *
   * компонентные функции:
   * split_string()               - разделение строки на слова
   * extract_digits_from_string() - последовательное извлечение цифр из строки
   */
  public:
    std::vector<std::string>
    split_string(const std::string &s)
      // функция разделяет строку на слова
    {
      std::stringstream ss(s);
      std::vector<std::string> res;
      std::string w {};
      while (ss >> w) { res.emplace_back(w); }
      return res;
    }

    std::vector<unsigned short int>
    extract_digits_from_string(const std::string &s)
      // функция последовательно извлекает цифры из строки
    {
      std::vector<unsigned short int> res {};
      for (char c : s) {
        if (std::isdigit(static_cast<unsigned char>(c))) {
          // преобразуем символ цифры в числовое значение
          res.emplace_back(c-'0');
        }
      }
      res.shrink_to_fit();
      return res;
    }
  };
}

#endif
