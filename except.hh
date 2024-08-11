/*
 * Copyright(C) 2024, S-Patriarch
 * Patriarch library : except.hh
 */

#ifndef PL_EXCEPT_HH
#define PL_EXCEPT_HH

#include <exception>

namespace pl {
  class Exception final : public std::exception {
    /*
     * работа с исключениями
     * класс наследуется от библиотечного класса - std::exception
     *
     * использование:
     * в коде программы бросается исключение следующим образом
     *   throw pl::Exception("E: Message to exception.");
     * ловится данное исключение следующим образом
     *   try {
     *     throw pl::Exception("E: Message to exception.");
     *   }
     *   catch (std::exception& ex) {
     *     std::cout << "\nПоймано исключение: " 
     *               << ex.what() 
     *               << std::endl;
     *   }
     */
  private:
    const char* m_message {};
  public:
    Exception(const char* mess) : std::exception(), m_message{mess} {}
    ~Exception() noexcept {}

    virtual const char* what() const noexcept override 
      // перегрузка метода what() стандартного класса std::exception
    { 
      return m_message; 
    }
  };
}

#endif
