/*
   (c) S-Patriarch, 2023
   (PL) Patriarch library : plconio.hh
   Работа с терминалом.
   Компонентные функции:
      clrscr() - очистка терминала и постановка курсора в верхний левый угол
      clreol() - удалает все символы после курсора до конца строки
      hcrs()   - скрыть курсор
      scrs()   - показать курсор
      gotoxy() - постановка курсора в координату x_ и y_ терминала
      wherex() - возвращает установленную координату text.s_cur_x
      wherey() - возвращает установленную координату text.s_cur_y
      getch()  - получение символа без ожидания нажатия enter <OS Linux>
      get_hidden_input()   - скрытый ввод в терминале OS Linux
      setlocaleWIN32rus()  - локализация консоли OS Windows для вывода
      setconsoleWIN32rus() - локализация консоли OS Windows для ввода/вывода
*/
#ifndef PL_PLCONIO_HH
#define PL_PLCONIO_HH
//------------------------------------------------------------------------------
#include <iostream>
//------------------------------------------------------------------------------
#ifdef __linux__
#include <termios.h>
#include <unistd.h>
#elif _WIN32
#include <clocale>
#include <windows.h>
#endif
//------------------------------------------------------------------------------
namespace pl
{
////////////////////////////////////////////////////////////////////////////////
class Conio
{
   struct textinfo
   {
      unsigned short s_cur_x{0};
      unsigned short s_cur_y{0};
   } text;
public:
//------------------------------------------------------------------------------
// Очистка терминала и постановка курсора в верхний левый угол.
//
   void clrscr()
   {
      std::cout
         <<"\033[2J\033[1;1H";
   }
//------------------------------------------------------------------------------
// Удаляет все символы, находящиеся после курсора и до конца строки, 
// но не удаляет символы, расположенные перед текущей позицией курсора.
//
   void clreol()
   {
      std::cout
         <<"\033[K";
   }
//------------------------------------------------------------------------------
// Скрыть курсор.
//
   void hcrs()
   {
      std::cout
         <<"\033[?25l";
   }
//------------------------------------------------------------------------------
// Показать курсор.
//
   void scrs()
   {
      std::cout
         <<"\033[?25h";
   }
//------------------------------------------------------------------------------
// Постановка курсора в координату x_ и y_ терминала.
//
   void gotoxy(unsigned short x_,unsigned short y_)
   {
      std::cout
         <<"\033["
         <<y_
         <<";"
         <<x_
         <<"H";
      text.s_cur_x=x_;
      text.s_cur_y=y_;
   }
//------------------------------------------------------------------------------
// Возвращает установленную координату text.s_cur_x.
//
   unsigned short wherex()
   {
      return text.s_cur_x;
   }
//------------------------------------------------------------------------------
// Возвращает установленную координату text.s_cur_y.
//
   unsigned short wherey()
   {
      return text.s_cur_y;
   }
//------------------------------------------------------------------------------
#ifdef __linux__
//------------------------------------------------------------------------------
// Получение символа с клавиатуры без ожидания нажатия enter.
// Аналог функции _getch() в стандартной библиотеке conio.h <OS Windows>.
// Реализация данной функции предназначена для работы в терминале OS Linux.
//
   signed getch()
   {
      signed charBuffer{0};
      struct termios old{0};
      if(tcgetattr(0,&old)<0)
      {
         perror("tcsetattr()");
      }
      old.c_lflag&=~ICANON;
      old.c_lflag&=~ECHO;
      old.c_cc[VMIN]=1;
      old.c_cc[VTIME]=0;
      if(tcsetattr(0,TCSANOW,&old)<0)
      {
         perror("tcsetattr ICANON");
      }
      if(read(0,&charBuffer,1)<0)
      {
         perror("read()");
      }
      old.c_lflag|=ICANON;
      old.c_lflag|=ECHO;
      if(tcsetattr(0,TCSADRAIN,&old)<0)
      {
         perror("tcsetattr ~ICANON");
      }
      return charBuffer;
   }
//------------------------------------------------------------------------------
// Функция скрытиго ввода в терминале OS Linux.
//
   std::string get_hidden_input()
   {
      std::string strResult{};
      struct termios old_t;
      struct termios new_t;
      // Сохраняем текущие настройки терминала.
      tcgetattr(STDIN_FILENO,&old_t);
      new_t=old_t;
      new_t.c_lflag&=~ECHO; // Отключаем отображение ввода.
      // Применяем новые настройки терминала.
      tcsetattr(STDIN_FILENO,TCSANOW,&new_t);
      // Читаем ввод пользователя.
      std::getline(std::cin,strResult);
      // Востанавливаем старые настройки терминала.
      tcsetattr(STDIN_FILENO,TCSANOW,&old_t);
      return strResult;
   }
#endif // __linux__
#ifdef _WIN32
//------------------------------------------------------------------------------
// Локализация консоли OS Windows для вывода текста на русском языке.
//
   void setlocaleWIN32rus()
   {
      setlocale(LC_ALL,"Russian");
   }
//------------------------------------------------------------------------------
// Локализация консоли OS Windows для ввода/вывода текста на русском языке.
//
   void setconsoleWIN32rus()
   {
      SetConsoleCP(1251);
      SetConsoleOutputCP(1251);
   }
#endif // _WIN32
}; // class pl::Conio
} // namespace pl
//------------------------------------------------------------------------------
#endif // PL_PLCONIO_HH

