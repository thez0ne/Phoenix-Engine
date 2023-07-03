#pragma once
#include "pch.h"

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Phoenix
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

#define ENGINE_TRACE(...) Phoenix::Log::GetEngineLog()->trace(__VA_ARGS__)
#define ENGINE_INFO(...) Phoenix::Log::GetEngineLog()->info(__VA_ARGS__)
#define ENGINE_WARN(...) Phoenix::Log::GetEngineLog()->warn(__VA_ARGS__)
#define ENGINE_ERR(...) Phoenix::Log::GetEngineLog()->error(__VA_ARGS__)
#define ENGINE_CRITICAL(...) Phoenix::Log::GetEngineLog()->critical(__VA_ARGS__)

#define DEBUG_LOG(...) Phoenix::Log::GetAppLog()->trace(__VA_ARGS__)
#define DEBUG_WARN(...) Phoenix::Log::GetAppLog()->warn(__VA_ARGS__)
#define DEBUG_ERR(...) Phoenix::Log::GetAppLog()->error(__VA_ARGS__)

#define PRINT(...) DEBUG_LOG(__VA_ARGS__)
