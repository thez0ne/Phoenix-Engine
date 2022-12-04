#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace Phoinix
{
   std::shared_ptr<spdlog::logger> Log::m_EngineLogger;
   std::shared_ptr<spdlog::logger> Log::m_AppLogger;

   void Log::Init()
   {
      spdlog::set_pattern("%^[%T][%n]: %v%$");

      m_EngineLogger = spdlog::stdout_color_mt("Phoinix");
      m_AppLogger = spdlog::stdout_color_mt("App");
      m_EngineLogger->set_level(spdlog::level::trace);
      m_AppLogger->set_level(spdlog::level::trace);
   }

   void Log::Shutdown() {}

   void Log::SetInfo()
   {
      m_EngineLogger->set_level(spdlog::level::info);
      m_AppLogger->set_level(spdlog::level::info);
   }

   void Log::SetTrace()
   {
      m_EngineLogger->set_level(spdlog::level::trace);
      m_AppLogger->set_level(spdlog::level::trace);
   }
}
