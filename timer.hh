/*
 * Copyright (C) 2024, S-Patriarch
 * Patriarch library : timer.hh
 */

#ifndef PL_TIMER_HH
#define PL_TIMER_HH 1

#include <chrono>

namespace pl {
  class Timer {
    /*
     * класс для замера времени работы кода
     *
     * компонентные функции:
     * start() - начало замера
     * stop()  - завершение замера
     * duration_milliseconds() - результат замера в миллисекундах
     * duration_seconds()      - результат замера в секундах
     */
  private:
    std::chrono::time_point<std::chrono::high_resolution_clock>  m_start_time;
    std::chrono::time_point<std::chrono::high_resolution_clock>  m_end_time;
  public:
    Timer() : m_start_time{std::chrono::high_resolution_clock::now()} {}
    
    void start() { m_start_time = std::chrono::high_resolution_clock::now(); }
    void stop() { m_end_time = std::chrono::high_resolution_clock::now(); }

    float duration_milliseconds() const
    {
      std::chrono::duration<float, std::milli> duration = m_end_time - m_start_time;
      return duration.count();
    }

    float duration_seconds() const
    {
      std::chrono::duration<float> duration = m_end_time - m_start_time;
      return duration.count();
    }
  };
}

#endif
