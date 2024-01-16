//
// (c) 2024 S-Patriarch
// Patriarch library : plbar.hh
//
#ifndef PL_PLBAR_HH
#define PL_PLBAR_HH

#ifndef PL_PLCOLOR_HH
#include "plcolor.hh"
#endif

#include <iostream>
#include <string>
#include <cstdint>

namespace pl {
   class Progress_bar {
      // индикаторы выполнения и работа с ними
      //
      // компонентные функции:
      // set_min() - установка минимальной процентной итерации
      // set_max() - установка максимальной процентной итерации
      // get_min() - получение минимальной процентной итерации
      // get_max() - получение максимальной процентной итерации
      // set_message() - установка сообщения индикатора
      // set_brackets() - установка границ индикатора
      // set_color() - установка цвета индикатора
      // set_style() - установка стиля индикатора
      // set_bar_width() - установка ширины индикатора
      // update() - обновление отрисовки индикатора выполнения
      //
      // использование:
      // {
      //    pl::Conio conio;
      //    pl::Progress_bar pb;
      //    pb.set_min(0);
      //    pb.set_max(100);
      //    pb.set_style("complete", "%", "#", ".");
      //    pb.set_message("выполнено...");
      //    pb.set_brackets("[", "]");
      //    pb.set_color(pl::Color::color::CURRENT, "tr");
      //    pb.set_bar_width(30);
      //    conio.crs_h();
      //    for (unsigned i=pb.get_min(); i<=pb.get_max(); ++i) {
      //       pb.update(i);
      //       // ... выполнение иных операций ...
      //    }
      //    conio.crs_s();
      // }
   private:
      std::uint16_t  m_minIteretions {0};
      std::uint16_t  m_maxIterations {100};
      std::uint16_t  m_barWidth {30};           // ширина индикатора
      
      pl::Color::color  m_barColor {pl::Color::color::CURRENT};
      std::string       m_barColorStyle {"tr"}; // tr - обычный цвет текста
                                                // tb - жырный цвет текста
      std::string  m_bar {"#"};           // символ отрисовки прогресса индикатора
      std::string  m_barSubstrate {" "};  // символ подложки прогресса индикатора
      std::string  m_style {"complete"};
      std::string  m_percent {"%"};
      std::string  m_message {""};
      std::string  m_bracketLeft {"["};
      std::string  m_bracketRigth {"]"};
   public:
      void set_min(std::uint16_t min_) {m_minIteretions = min_;}
      void set_max(std::uint16_t max_) {m_maxIterations = max_;}
      std::uint16_t get_min() {return m_minIteretions;}
      std::uint16_t get_max() {return m_maxIterations;}
      void set_message(std::string message) {m_message = message;}
      void set_brackets(std::string bracket_left, std::string bracket_rigth)
      {
         m_bracketLeft = bracket_left;
         m_bracketRigth = bracket_rigth;
      }
      void set_color(pl::Color::color color_, std::string color_text = "tr")
      {
         m_barColorStyle = color_text;
         m_barColor = color_;
      }
      void set_style(std::string style, std::string percent)
      {
         m_style = style;
         m_percent = percent;
      }
      void set_style(std::string style, std::string percent,
                     std::string bar, std::string bar_substrate)
      {
         m_style = style;
         m_percent = percent;
         m_bar = bar;
         m_barSubstrate = bar_substrate;
      }
      void set_bar_width(std::uint16_t width) {m_barWidth = width;}
      
      void update(std::uint16_t index_)
         // обновление отрисовки индикатора выполнения
      {
         // вычисление процента выполнения
         float progress_ = static_cast<float>(index_)/m_maxIterations;
         std::uint16_t completedWidth_ =
            static_cast<std::uint16_t>(progress_*m_barWidth);
         pl::Color c;
         // вывод индикатора выполнения
         std::cout << m_bracketLeft;
         for (std::uint16_t i=0; i<=m_barWidth; ++i) {
            if (i<=completedWidth_) {
               if (m_barColor==pl::Color::color::CURRENT) std::cout << m_bar;
               else {
                  if (m_barColorStyle=="tr") std::cout << c.esc_tr(m_barColor);
                  else if (m_barColorStyle=="tb") std::cout << c.esc_tb(m_barColor);
                  std::cout << m_bar << c.esc_c();
               }
            }
            else 
               std::cout << m_barSubstrate;
         }
         std::cout << m_bracketRigth << ' '
                   << (std::uint16_t)(progress_*100.0)
                   << m_percent << ' '
                   << m_message
                   << '\r';
         std::cout.flush();
      }
   };
}
#endif // PL_PLBAR_HH
