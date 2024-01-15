//
// (c) 2024 S-Patriarch
// Patriarch library : plconio.hh
//
#ifndef PL_PLCONIO_HH
#define PL_PLCONIO_HH

#include <iostream>
#include <cstdint>

#ifdef __linux__
#include <termios.h>
#include <unistd.h>
#elif _WIN32
#include <clocale>
#include <windows.h>
#endif

namespace pl {
   class Conio {
      // работа с терминалом
      //
      // компонентные функции:
      // clr_scr() - очистка терминала и постановка курсора в верхний левый угол
      // clr_eol() - удалает все символы после курсора до конца строки
      // crs_h()   - скрыть курсор
      // crs_s()   - показать курсор
      // goto_xy() - постановка курсора в координату x_ и y_ терминала
      // where_x() - возвращает установленную координату text.s_cur_x
      // where_y() - возвращает установленную координату text.s_cur_y
      // get_ch()  - получение символа без ожидания нажатия enter <OS Linux>
      // get_hidden_input()   - скрытый ввод в терминале OS Linux
      // set_locale_WIN32_rus()  - локализация консоли OS Windows для вывода
      // set_console_WIN32_rus() - локализация консоли OS Windows для ввода/вывода
   private:
      struct Text_info {
         std::uint16_t x {0};
         std::uint16_t y {0};
      } text;
   public:
      void clr_scr() {std::cout << "\033[2J\033[1;1H";}
      void clr_eol() {std::cout << "\033[K";}
      void crs_h()   {std::cout << "\033[?25l";}
      void crs_s()   {std::cout << "\033[?25h";}
      void goto_xy(std::uint16_t x, std::uint16_t y)
      {
         std::cout << "\033[" << y << ";" << x << "H";
         text.x = x;
         text.y = y;
      }
      std::uint16_t where_x() {return text.x;}
      std::uint16_t where_y() {return text.y;}
#ifdef __linux__
      std::int32_t get_ch()
         // получение символа с клавиатуры без ожидания нажатия enter
         // аналог функции _getch() в стандартной библиотеке conio.h <OS Windows>
         // реализация данной функции предназначена для работы в терминале OS Linux
      {
         std::int32_t buf {0};
         struct termios old {0};
         if (tcgetattr(0, &old)<0) perror("tcsetattr()");
         old.c_lflag &= ~ICANON;
         old.c_lflag &= ~ECHO;
         old.c_cc[VMIN] = 1;
         old.c_cc[VTIME] = 0;
         if (tcsetattr(0, TCSANOW, &old)<0) perror("tcsetattr ICANON");
         if (read(0, &buf, 1)<0) perror("read()");
         old.c_lflag |= ICANON;
         old.c_lflag |= ECHO;
         if (tcsetattr(0, TCSADRAIN, &old)<0) perror("tcsetattr ~ICANON");
         return buf;
      }
      std::string get_hidden_input()
      {
         std::string s_res {};
         struct termios old_t;
         struct termios new_t;
         // сохраняем текущие настройки терминала
         tcgetattr(STDIN_FILENO, &old_t);
         new_t = old_t;
         new_t.c_lflag &= ~ECHO; // отключаем отображение ввода
         // применяем новые настройки терминала
         tcsetattr(STDIN_FILENO, TCSANOW, &new_t);
         // читаем ввод пользователя
         std::getline(std::cin, s_res);
         // востанавливаем старые настройки терминала
         tcsetattr(STDIN_FILENO, TCSANOW, &old_t);
         return s_res;
      }
#endif // __linux__
#ifdef _WIN32
      void set_locale_WIN32_rus() {setlocale(LC_ALL,"Russian");}
      void set_console_WIN32_rus() 
      {
         SetConsoleCP(1251);
         SetConsoleOutputCP(1251);
      }
#endif // _WIN32
   };
} 
#endif // PL_PLCONIO_HH
