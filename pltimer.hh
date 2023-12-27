/*
   (c) S-Patriarch, 2023
   (PL) Patriarch library : pltimer.hh
   Класс pl::Timer для замера времени работы кода.
   Компонентные функции:
      start() - начало замера
      stop()  - завершение замера
      duration_milliseconds() - результат замера в миллисекундах
      duration_seconds()      - результат замера в секундах
*/
#ifndef PL_PLTIMER_HH
#define PL_PLTIMER_HH
//------------------------------------------------------------------------------
#include <chrono>
//------------------------------------------------------------------------------
namespace pl
{
////////////////////////////////////////////////////////////////////////////////
class Timer
{
   std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
   std::chrono::time_point<std::chrono::high_resolution_clock> m_endTime;
public:
   Timer(): 
      m_startTime{std::chrono::high_resolution_clock::now()}
   {}
//------------------------------------------------------------------------------
   void start()
   { 
      m_startTime=std::chrono::high_resolution_clock::now(); 
   }
//------------------------------------------------------------------------------
   void stop()
   { 
      m_endTime=std::chrono::high_resolution_clock::now(); 
   }
//------------------------------------------------------------------------------
   float duration_milliseconds() const
   {
      std::chrono::duration<float,std::milli> duration=m_endTime-m_startTime;
      return duration.count();
   }
//------------------------------------------------------------------------------
   float duration_seconds() const
   {
      std::chrono::duration<float> duration=m_endTime-m_startTime;
      return duration.count();
   }
}; // class pl::Timer
} // namespace pl
//------------------------------------------------------------------------------
#endif // PL_PLTIMER_HH

