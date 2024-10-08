/*
 * Copyright (C) 2024, S-Patriarch
 * Patriarch library : color.hh
 */

#ifndef PL_COLOR_HH
#define PL_COLOR_HH 1

#include <string>
#include <ostream>

#ifdef _WIN32
#include <windows.h>
#endif

namespace pl {
  namespace mr {
    /*
     * встраиваемые функции: 
     * reset       - выключение всех атрибутов
     * bold        - жирный или увеличить яркость
     * faint       - блеклый или уменьшить яркость
     * italic      - курсив
     * underline   - подчеркнутый один раз
     * crossed_out - зачеркнутый
     * reset_fg    - цвет текста по умолчанию на переднем плане
     * reset_bg    - цвет фона по умолчанию
     *
     * использование:
     * {
     *   std::cout << pl::mr::bold << "текст" << pl::mr::reset;
     * }
     */

    inline std::ostream& reset(std::ostream& os) { return os << "\033[0m"; }
    inline std::ostream& bold(std::ostream& os) { return os << "\033[1m"; }  
    inline std::ostream& faint(std::ostream& os) { return os << "\033[2m"; }
    inline std::ostream& italic(std::ostream& os) { return os << "\033[3m"; }
    inline std::ostream& underline(std::ostream& os) { return os << "\033[4m"; }
    inline std::ostream& inverse(std::ostream& os) { return os << "\033[7m"; }
    inline std::ostream& crossed_out(std::ostream& os) { return os << "\033[9m"; }
    inline std::ostream& reset_fg(std::ostream& os) { return os << "\033[39m"; }
    inline std::ostream& reset_bg(std::ostream& os) { return os << "\033[49m"; }
  }

  class Color {
    /*
     * работа с цветовой esc-последовательностью в терминале
     *
     * компонентные функции:
     * esc_c  - сбрасывает цветовую esc-последовательность
     * esc_tr - устанавливает обычный цвет текста
     * esc_tb - устанавливает жирный цвет текста
     * esc_br - устанавливает обычный цвет фона
     * esc_bb - устанавливает жирный цвет фона
     * get_bkg_color - получение цвета фона терминала
     *
     * использование:
     * {
     *   pl::Color c;
     *   std::cout << c.esc_tb(pl::Color::color::CYAN) << 'W' << c.esc_c()
     *             <<": тест использования нового класса pl::Color\n";
     * }
     */
  private:
    unsigned short int  m_sizeArray {8};
    std::string*        m_textColorRegular; 
    std::string*        m_textColorBold;    
    std::string*        m_backgroundColorRegular;
    std::string*        m_backgroundColorBold;
    std::string         m_colorClear;
  public:
    Color()
    { 
      m_colorClear = "\033[0m";
      m_textColorRegular = new std::string[m_sizeArray] { 
        "\033[30m", "\033[31m", "\033[32m", "\033[33m", 
        "\033[34m", "\033[35m", "\033[36m", "\033[37m"
      };
      m_textColorBold = new std::string[m_sizeArray] { 
        "\033[30;1m", "\033[31;1m", "\033[32;1m", "\033[33;1m", 
        "\033[34;1m", "\033[35;1m", "\033[36;1m", "\033[37;1m"
      };
      m_backgroundColorRegular = new std::string[m_sizeArray] { 
        "\033[40m", "\033[41m", "\033[42m", "\033[43m", 
        "\033[44m", "\033[45m", "\033[46m", "\033[47m"
      };
      m_backgroundColorBold = new std::string[m_sizeArray] { 
        "\033[40;1m", "\033[41;1m", "\033[42;1m", "\033[43;1m", 
        "\033[44;1m", "\033[45;1m", "\033[46;1m", "\033[47;1m"
      };
    }

    virtual ~Color() noexcept
    { 
      delete[] m_textColorRegular;
      delete[] m_textColorBold; 
      delete[] m_backgroundColorRegular;
      delete[] m_backgroundColorBold;
      
      m_textColorRegular = nullptr;
      m_textColorBold = nullptr; 
      m_backgroundColorRegular = nullptr;
      m_backgroundColorBold = nullptr;
    }

    enum color {
      BLACK = 0, 
      RED = 1, 
      GREEN = 2, 
      YELLOW = 3, 
      BLUE = 4, 
      MAGENTA = 5, 
      CYAN = 6, 
      WHITE = 7,
      CURRENT = 1000
    };

    std::string esc_c()         const { return m_colorClear; }
    std::string esc_tr(color i) const { return m_textColorRegular[i]; }
    std::string esc_tb(color i) const { return m_textColorBold[i]; }
    std::string esc_br(color i) const { return m_backgroundColorRegular[i]; }
    std::string esc_bb(color i) const { return m_backgroundColorBold[i]; }
    
    color get_bkg_color() const
    {
      color backgroundColor;
#ifdef __linux__
      backgroundColor = color::BLACK;
#elif _WIN32
      HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
      CONSOLE_SCREEN_BUFFER_INFO csbi;
      // получаем информацию о текущих атрибутах экранного буфера
      GetConsoleScreenBufferInfo(hConsole, &csbi);
      // извлекаем цвет фона
      WORD backgroundColor = csbi.wAttributes & 0xF0;
      backgroundColor = static_cast<color>(backgroundColor);
#endif
      if (backgroundColor != color::BLACK   
        && backgroundColor != color::RED      
        && backgroundColor != color::GREEN    
        && backgroundColor != color::YELLOW   
        && backgroundColor != color::BLUE     
        && backgroundColor != color::MAGENTA  
        && backgroundColor != color::CYAN     
        && backgroundColor != color::WHITE) {
        backgroundColor = color::BLACK;
      }
      return backgroundColor;
    }
  };
}  

#endif
