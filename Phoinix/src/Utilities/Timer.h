#pragma once
#include "pch.h"

#include <chrono>

namespace Phoinix::Utils
{
  enum class TimeFormat
  {
    Ms = 0,
    S,
    Ns
  };

  class Timer
  {
  public:
    Timer() { Reset(); }
    ~Timer() = default;

    [[nodiscard]] float ElapsedNanoSeconds() const
    {
      return std::chrono::duration_cast<std::chrono::nanoseconds>(
               std::chrono::high_resolution_clock::now() - m_Start)
        .count();
    }

    [[nodiscard]] float ElapsedMilliSeconds() const
    {
      return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::high_resolution_clock::now() - m_Start)
        .count();
    }

    [[nodiscard]] float ElapsedSeconds() const
    {
      return std::chrono::duration_cast<std::chrono::seconds>(
               std::chrono::high_resolution_clock::now() - m_Start)
        .count();
    }

    void Reset() { m_Start = std::chrono::high_resolution_clock::now(); }

  private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
  };

  class ScopedTimer
  {
  public:
    ScopedTimer(const std::string& name, TimeFormat format = TimeFormat::Ms) :
      m_Name(name), m_Timer(), m_Format(format)
    {}

    ~ScopedTimer()
    {
      // TODO do something with this value other than printing?
      switch (m_Format)
      {
        case TimeFormat::Ms:
          ENGINE_TRACE(
            "{}'s time has ended with duration {} ms", m_Name, m_Timer.ElapsedMilliSeconds());
          break;
        case TimeFormat::S:
          ENGINE_TRACE("{}'s time has ended with duration {} s", m_Name, m_Timer.ElapsedSeconds());
          break;
        case TimeFormat::Ns:
          ENGINE_TRACE(
            "{}'s time has ended with duration {} ns", m_Name, m_Timer.ElapsedNanoSeconds());
          break;
      }
    }

  private:
    std::string m_Name;
    Timer m_Timer;
    TimeFormat m_Format;
  };
}
