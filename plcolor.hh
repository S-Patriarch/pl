/*
   (c) S-Patriarch, 2023
   (PL) Patriarch library : plcolor.hh

   Работа с цветовой esc-последовательностью в терминале.

Компонентные функции:
   esc_c  - сбрасывает цветовую esc-последовательность
   esc_tr - устанавливает обычный цвет текста
   esc_tb - устанавливает жирный цвет текста
   esc_br - устанавливает обычный цвет фона
   esc_bb - устанавливает жирный цвет фона
   getbkgcolor - получение цвета фона терминала
*/

#ifndef PL_PLCOLOR_HH
#define PL_PLCOLOR_HH

#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

namespace pl
{
//------------------------------------------------------------------------------
enum eColor 
{
   BLACK=0,RED=1,GREEN=2,YELLOW=3,BLUE=4,MAGENTA=5,CYAN=6,WHITE=7,CURRENT=1000
};
////////////////////////////////////////////////////////////////////////////////
class Color 
{
   unsigned short int m_sizeArray{8};
   std::string* m_textColorRegular; 
   std::string* m_textColorBold;    
   std::string* m_backgroundColorRegular;
   std::string* m_backgroundColorBold;
   std::string  m_colorClear;
public:
   Color()
   { 
      m_colorClear="\033[0m";
      m_textColorRegular=new std::string[m_sizeArray]{ 
         "\033[30m","\033[31m","\033[32m","\033[33m", 
         "\033[34m","\033[35m","\033[36m","\033[37m"};
      m_textColorBold=new std::string[m_sizeArray]{ 
         "\033[30;1m","\033[31;1m","\033[32;1m","\033[33;1m", 
         "\033[34;1m","\033[35;1m","\033[36;1m","\033[37;1m"};
      m_backgroundColorRegular=new std::string[m_sizeArray]{ 
         "\033[40m","\033[41m","\033[42m","\033[43m", 
         "\033[44m","\033[45m","\033[46m","\033[47m"};
      m_backgroundColorBold=new std::string[m_sizeArray]{ 
         "\033[40;1m","\033[41;1m","\033[42;1m","\033[43;1m", 
         "\033[44;1m","\033[45;1m","\033[46;1m","\033[47;1m"};
   }
   virtual
   ~Color() noexcept
   { 
      delete[] m_textColorRegular;
      delete[] m_textColorBold; 
      delete[] m_backgroundColorRegular;
      delete[] m_backgroundColorBold;
   }
   //---------------------------------------------------------------------------
   // Возвращает строку сброса цветовой esc-последовательности.
   //
   std::string 
   esc_c() 
   { 
      return m_colorClear; 
   }
   //---------------------------------------------------------------------------
   // Устанавливает цвет текста.
   // Возвращает строку цветовой esc-последовательности обычной
   // насыщенности по заданному index_ от 0 до 7 включительно.
   //
   std::string 
   esc_tr(eColor index_)
   { 
      return m_textColorRegular[index_]; 
   }
   //---------------------------------------------------------------------------
   // Устанавливает цвет текста.
   // Возвращает строку цветовой esc-последовательности жирной
   // насыщенности по заданному index_ от 0 до 7 включительно.
   //
   std::string 
   esc_tb(eColor index_)
   { 
      return m_textColorBold[index_]; 
   }
   //---------------------------------------------------------------------------
   // Устанавливает цвет фона.
   // Возвращает строку цветовой esc-последовательности обычной
   // насыщенности по заданному index_ от 0 до 7 включительно.
   //
   std::string 
   esc_br(eColor index_ )
   { 
      return m_backgroundColorRegular[index_]; 
   }
   //---------------------------------------------------------------------------
   // Устанавливает цвет фона.
   // Возвращает строку цветовой esc-последовательности жирной
   // насыщенности по заданному index_ от 0 до 7 включительно.
   //
   std::string 
   esc_bb(eColor index_)
   { 
      return m_backgroundColorBold[index_]; 
   }
   //---------------------------------------------------------------------------
   // Получение цвета фона терминала (Linux), консоли (Windows).
   // Возвращает индекс цвета перечисления eColor.
   //
   eColor 
   getbkgcolor()
   {
      eColor backgroundColor;

      #ifdef __linux__
         backgroundColor=eColor::BLACK;
      #elif _WIN32
         HANDLE hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
         CONSOLE_SCREEN_BUFFER_INFO csbi;
         // Получаем информацию о текущих атрибутах экранного буфера.
         GetConsoleScreenBufferInfo(hConsole,&csbi);
         // Извлекаем цвет фона.
         WORD backgroundColor=csbi.wAttributes&0xF0;
         backgroundColor=static_cast<Color>(backgroundColor);
      #endif

      if(backgroundColor!=eColor::BLACK  && 
         backgroundColor!=eColor::RED    && 
         backgroundColor!=eColor::GREEN  && 
         backgroundColor!=eColor::YELLOW && 
         backgroundColor!=eColor::BLUE   && 
         backgroundColor!=eColor::MAGENTA&& 
         backgroundColor!=eColor::CYAN   && 
         backgroundColor!=eColor::WHITE) 
      {
         backgroundColor=eColor::BLACK;
      }
      return backgroundColor;
   }
}; // class pl::Color
} // namespace pl 

#endif // PL_PLCOLOR_HH

