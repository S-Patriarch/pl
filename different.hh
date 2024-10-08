/*
 * Copyright(C) 2024, S-Patriarch
 * Patriarch library : different.hh
 */

#ifndef PL_DIFFERENT_HH
#define PL_DIFFERENT_HH 1

#include <cstdint>
#include <vector>
#include <sstream>
#include <cctype>

namespace pl {
  /*
   * библиотека всяких разных функций
   *
   * функции:
   * pack_bools()                 - упаковка 8 значений bool в 1 байт
   * unpack_bools()               - распаковка 1 байта на 8 значений типа bool
   * split_string()               - разделение строки на слова
   * extract_digits_from_string() - последовательное извлечение цифр из строки
   */
   
  std::uint8_t pack_bools(bool b1, bool b2, bool b3, bool b4,
                          bool b5, bool b6, bool b7, bool b8) 
    // функция упаковывает 8 значений типа bool в
    // переменную размером 1 байт
    // это значительно может сэкономить потребление памяти, поскольку
    // в логическом типе данных bool есть лишь два возможных значения
    // (true и false), которые могут быть представлены одним битом,
    // но по факту занимают целый байт памяти
    // а это, в свою очередь, из-за того, что переменные используют
    // уникальные адреса памяти, а они выделяются только в байтах
    // переменная bool занимает 1 бит, а другие 7 бит - тратятся впустую
  {
    std::uint8_t p {0};
    p |= b1 << 7;
    p |= b2 << 6;
    p |= b3 << 5;
    p |= b4 << 4;
    p |= b5 << 3;
    p |= b6 << 2;
    p |= b7 << 1;
    p |= b8;
    return p;
  }

  void unpack_bools(std::uint8_t p, bool& b1, bool& b2, 
                    bool& b3, bool& b4, bool& b5, 
                    bool& b6, bool& b7, bool& b8) 
    // функция распаковывает 1 байт на 8 значений типа bool
  {
    b1 = (p >> 7) & 1;
    b2 = (p >> 6) & 1;
    b3 = (p >> 5) & 1;
    b4 = (p >> 4) & 1;
    b5 = (p >> 3) & 1;
    b6 = (p >> 2) & 1;
    b7 = (p >> 1) & 1;
    b8 = p & 1;
  }

  std::vector<std::string> split_string(const std::string& s) 
    // функция разделяет строку на слова
  {
    std::stringstream ss(s);
    std::vector<std::string> res;
    std::string w {};
    while (ss >> w) { res.emplace_back(w); }
    return res;
  }

  std::vector<unsigned short int> 
  extract_digits_from_string(const std::string& s) 
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
}

#endif
