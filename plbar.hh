/*
   (c) S-Patriarch, 2023
   (PL) Patriarch library : plbar.hh

   Индикаторы выполнения и работа с ними.

Компонентные функции:
   set_min() - установка минимальной процентной итерации
   set_max() - установка максимальной процентной итерации
   get_min() - получение минимальной процентной итерации
   get_max() - получение максимальной процентной итерации
   set_message() - установка сообщения индикатора
   set_brackets() - установка границ индикатора
   set_color() - установка цвета индикатора
   set_style() - установка стиля индикатора
   set_bar_width() - установка ширины индикатора
   update() - обновление отрисовки индикатора выполнения
   
Использование:
   {
      pl::Conio conio;

      pl::ProgressBar pb;
      pb.set_min(0);
      pb.set_max(100);
      pb.set_style("complete","%","#",".");
      pb.set_message("выполнено...");
      pb.set_brackets("[","]");
      pb.set_color(pl::Color::color::CURRENT,"tr");
      pb.set_bar_width(30);

      conio.hcrs();
      for(unsigned i{pb.get_min()};i<=pb.get_max();++i)
      {
         pb.update(i);
         // ... выполнение иных операций ...
      }
      conio.scrs();
   }
*/

#ifndef PL_PLBAR_HH
#define PL_PLBAR_HH

#ifndef PL_PLCOLOR_HH
#include "plcolor.hh"
#endif

#include <iostream>
#include <string>

namespace pl
{
////////////////////////////////////////////////////////////////////////////////
class ProgressBar
{
   unsigned       m_minIteretions{0};
   unsigned       m_maxIterations{100};
   unsigned short m_barWidth{30}; // Ширина индикатора.

   pl::Color::color m_barColor{pl::Color::color::CURRENT};
   std::string m_barColorStyle{"tr"}; // tr - обычный цвет текста
                                      // tb - жырный цвет текста
   std::string m_bar{"#"};          // Символ отрисовки прогресса индикатора.
   std::string m_barSubstrate{" "}; // Символ подложки прогресса индикатора.
   std::string m_style{"complete"};
   std::string m_percent{"%"};
   std::string m_message{""};
   std::string m_bracketLeft{"["};
   std::string m_bracketRigth{"]"};
public:
   //---------------------------------------------------------------------------
   // Установка минимальной процентной итерации.
   //
   void
   set_min(unsigned min_)
   {
      m_minIteretions=min_;
   }
   //---------------------------------------------------------------------------
   // Установка максимальной процентной итерации.
   //
   void
   set_max(unsigned max_)
   {
      m_maxIterations=max_;
   }
   //---------------------------------------------------------------------------
   // Получение минимальной процентной итерации.
   //
   unsigned
   get_min()
   {
      return m_minIteretions;
   }
   //---------------------------------------------------------------------------
   // Получение максимальной процентной итерации.
   //
   unsigned
   get_max()
   {
      return m_maxIterations;
   }
   //---------------------------------------------------------------------------
   // Установка сообщения индикатора.
   //
   void
   set_message(std::string message_)
   {
      m_message=message_;
   }
   //---------------------------------------------------------------------------
   // Установка границ индикатора.
   //
   void
   set_brackets(std::string bracketLeft_,std::string bracketRigth_)
   {
      m_bracketLeft=bracketLeft_;
      m_bracketRigth=bracketRigth_;
   }
   //---------------------------------------------------------------------------
   // Установка цвета индикатора.
   //
   void
   set_color(pl::Color::color color_,std::string colorText="tr")
   {
      m_barColorStyle=colorText;
      m_barColor=color_;
   }
   //---------------------------------------------------------------------------
   // Установка стиля индикатора.
   // Перегруженная функция.
   //
   void
   set_style(std::string style_,std::string percent_)
   {
      m_style=style_;
      m_percent=percent_;
   }
   //---------------------------------------------------------------------------
   // Установка стиля индикатора.
   // Перегруженная функция.
   //
   void
   set_style(std::string style_,std::string percent_,
             std::string bar_,std::string barSubstrate)
   {
      m_style=style_;
      m_percent=percent_;
      m_bar=bar_;
      m_barSubstrate=barSubstrate;
   }
   //---------------------------------------------------------------------------
   // Установка ширины индикатора.
   //
   void
   set_bar_width(unsigned short width_)
   {
      m_barWidth=width_;
   }
   //---------------------------------------------------------------------------
   // Обновление отрисовки индикатора выполнения.
   //
   void
   update(unsigned index_)
   {
      // Вычисление процента выполнения.
      float progress_=static_cast<float>(index_)/m_maxIterations;
      unsigned short completedWidth_=
         static_cast<unsigned short>(progress_*m_barWidth);

      pl::Color c;

      // Вывод индикатора выполнения.
      std::cout
         <<m_bracketLeft;

      for(unsigned short i=0;i<=m_barWidth;++i)
      {
         if(i<=completedWidth_)
         {
            if(m_barColor==pl::Color::color::CURRENT)
            {
               std::cout
                  <<m_bar;
            }
            else
            {
               if(m_barColorStyle=="tr")
               {
                  std::cout
                     <<c.esc_tr(m_barColor);
               }
               else if(m_barColorStyle=="tb")
               {
                  std::cout
                     <<c.esc_tb(m_barColor);
               }
               std::cout
                  <<m_bar
                  <<c.esc_c();
            }
         }
         else
         {
            std::cout
               <<m_barSubstrate;
         }
      }
      std::cout
         <<m_bracketRigth
         <<" "
         <<(unsigned short)(progress_*100.0)
         <<m_percent
         <<" "
         <<m_message
         <<"\r";
      std::cout.flush();
   }
}; // class ProgressBar
} // namespace pl

#endif // PL_PLBAR_HH

