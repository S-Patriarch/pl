/*
   (c) S-Patriarch, 2023
   (PL) Patriarch library : plexcept.hh

   Работа с исключениями.
   Класс наследуется от библиотечного класса - std::exception.

Использование:
   Пример использования данного класса исключений.
   В коде программы бросается исключение следующим образом.

   throw pl::Exception("E: Message to exception.");
 
   Ловится данное исключение следующим образом.

   try 
   {
      throw pl::Exception("E: Message to exception.");
   }
   catch(std::exception& ex) 
   {
      std::cout 
         <<"\nПоймано исключение: " 
         <<ex.what() 
         <<'\n';
   }
*/

#ifndef PL_PLEXCEPT_HH
#define PL_PLEXCEPT_HH

#include <exception>

namespace pl
{
////////////////////////////////////////////////////////////////////////////////
class Exception final: public std::exception
{
   const char* m_message{}; // Сообщение для исключения.
public:
   Exception(const char* message_): 
      std::exception(), 
      m_message{message_} 
   {}
   ~Exception() noexcept 
   {}
   //---------------------------------------------------------------------------
   // Перегрузка метода what() стандартного класса std::exception.
   //
   virtual 
   const char* 
   what() const noexcept override 
   { 
      return m_message; 
   }
}; // class pl::Exception
} // namespace pl

#endif // PL_PLEXCEPT_HH

