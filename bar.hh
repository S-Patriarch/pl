/*
 * Copyright(C) 2024, S-Patriarch
 * Patriarch library : bar.hh
 */

#ifndef PL_BAR_HH
#define PL_BAR_HH 1

#ifndef PL_COLOR_HH
#include "color.hh"
#endif

#include <iostream>
#include <string>

namespace pl {
  class ProgressBar {
    /*
     * индикаторы выполнения и работа с ними
     *
     * компонентные функции:
     * set_min() - установка минимальной процентной итерации
     * set_max() - установка максимальной процентной итерации
     * get_min() - получение минимальной процентной итерации
     * get_max() - получение максимальной процентной итерации
     * set_message()  - установка сообщения индикатора
     * set_brackets() - установка границ индикатора
     * set_color() - установка цвета индикатора
     * set_style() - установка стиля индикатора
     * set_bar_width() - установка ширины индикатора
     * update() - обновление отрисовки индикатора выполнения
     *
     * использование:
     * {
     *   pl::Conio conio;
     *   pl::ProgressBar pb;
     *   pb.set_min(0);
     *   pb.set_max(100);
     *   pb.set_style("complete", "%", "#", ".");
     *   pb.set_message("выполнено...");
     *   pb.set_brackets("[", "]");
     *   pb.set_color(pl::Color::color::CURRENT, "tr");
     *   pb.set_bar_width(30);
     *   conio.crs_h();
     *   for (unsigned i=pb.get_min(); i<=pb.get_max(); ++i) {
     *     pb.update(i);
     *     // ... выполнение иных операций ...
     *   }
     *   conio.crs_s();
     * }
     */
  private:
    unsigned short int  _minIteretions {0};
    unsigned short int  _maxIterations {100};
    unsigned short int  _barWidth {30};      // ширина индикатора
      
    pl::Color::color  _barColor {pl::Color::color::CURRENT};
    std::string       _barColorStyle {"tr"}; // tr - обычный цвет текста
                                             // tb - жырный цвет текста
    std::string  _bar {"#"};           // символ отрисовки прогресса индикатора
    std::string  _barSubstrate {" "};  // символ подложки прогресса индикатора
    std::string  _style {"complete"};
    std::string  _percent {"%"};
    std::string  _message {""};
    std::string  _bracketLeft {"["};
    std::string  _bracketRigth {"]"};
  public:
    void set_min(unsigned short int min_) { _minIteretions = min_; }
    void set_max(unsigned short int max_) { _maxIterations = max_; }
    unsigned short int get_min() const { return _minIteretions; }
    unsigned short int get_max() const { return _maxIterations; }
    void set_message(std::string message) { _message = message; }
    
    void set_brackets(std::string bracket_left, std::string bracket_rigth)
    {
      _bracketLeft = bracket_left;
      _bracketRigth = bracket_rigth;
    }

    void set_color(pl::Color::color color_, std::string color_text = "tr")
    {
       _barColorStyle = color_text;
       _barColor = color_;
    }

    void set_style(std::string style, std::string percent)
    {
      _style = style;
      _percent = percent;
    }

    void set_style(std::string style,std::string percent,
                   std::string bar, std::string bar_substrate)
    {
      _style = style;
      _percent = percent;
      _bar = bar;
      _barSubstrate = bar_substrate;
    }

    void set_bar_width(unsigned short int width) { _barWidth = width; }
      
    void update(unsigned short int index_) const
      // обновление отрисовки индикатора выполнения
    {
      // вычисление процента выполнения
      float progress_ = static_cast<float>(index_)/_maxIterations;
      unsigned short int completedWidth_ =
      static_cast<unsigned short int>(progress_*_barWidth);
      
      pl::Color c;
      // вывод индикатора выполнения
      std::cout << _bracketLeft;
      
      for (unsigned short int i=0; i<=_barWidth; ++i) {
        if (i <= completedWidth_) {
          if (_barColor == pl::Color::color::CURRENT) std::cout << _bar;
          else {
            if (_barColorStyle == "tr") std::cout << c.esc_tr(_barColor);
            else if (_barColorStyle == "tb") std::cout << c.esc_tb(_barColor);
              std::cout << _bar << pl::mr::reset;
          }
        }
        else 
          std::cout << _barSubstrate;
      }

      std::cout << _bracketRigth << ' '
                << (unsigned short int)(progress_ * 100.0)
                << _percent << ' '
                << _message
                << '\r';
      std::cout.flush();
    }
  };
}

#endif
