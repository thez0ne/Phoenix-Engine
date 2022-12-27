#pragma once
#include "pch.h"

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Phoinix
{
  class Log
  {
  public:
    static void Init();
    static void Shutdown();
    static void SetInfo();
    static void SetTrace();

    static std::shared_ptr<spdlog::logger>& GetEngineLog() { return m_EngineLogger; }
    static std::shared_ptr<spdlog::logger>& GetAppLog() { return m_AppLogger; }

  private:
    static std::shared_ptr<spdlog::logger> m_EngineLogger;
    static std::shared_ptr<spdlog::logger> m_AppLogger;
  };
}

#define ENGINE_TRACE(...) Phoinix::Log::GetEngineLog()->trace(__VA_ARGS__)
#define ENGINE_INFO(...) Phoinix::Log::GetEngineLog()->info(__VA_ARGS__)
#define ENGINE_WARN(...) Phoinix::Log::GetEngineLog()->warn(__VA_ARGS__)
#define ENGINE_ERR(...) Phoinix::Log::GetEngineLog()->error(__VA_ARGS__)
#define ENGINE_CRITICAL(...) Phoinix::Log::GetEngineLog()->critical(__VA_ARGS__)

#define DEBUG_LOG(...) Phoinix::Log::GetAppLog()->trace(__VA_ARGS__)
#define DEBUG_WARN(...) Phoinix::Log::GetAppLog()->warn(__VA_ARGS__)
#define DEBUG_ERR(...) Phoinix::Log::GetAppLog()->error(__VA_ARGS__)

#define PRINT(...) DEBUG_LOG(__VA_ARGS__)
