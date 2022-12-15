#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/daily_file_sink.h"

namespace Phoinix
{
   std::shared_ptr<spdlog::logger> Log::m_EngineLogger;
   std::shared_ptr<spdlog::logger> Log::m_AppLogger;

   void Log::Init()
   {
      spdlog::set_pattern("%^[%T][%n]: %v%$");

      std::vector<spdlog::sink_ptr> engineSinks;
      engineSinks.resize(2);
      engineSinks[0] = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
      engineSinks[0]->set_pattern("%^[%T][%n]: %v%$");
      engineSinks[1] = std::make_shared<spdlog::sinks::daily_file_sink_mt>("engine.log", 23, 59, true);
      m_EngineLogger = std::make_shared<spdlog::logger>("Phoinix", engineSinks.begin(), engineSinks.end());

      std::vector<spdlog::sink_ptr> appSinks;
      appSinks.resize(2);
      appSinks[0] = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
      appSinks[0]->set_pattern("%^[%T][%n]: %v%$");
      appSinks[1] = std::make_shared<spdlog::sinks::daily_file_sink_mt>("app.log", 23, 59);
      m_AppLogger = std::make_shared<spdlog::logger>("App", appSinks.begin(), appSinks.end());

      m_EngineLogger->set_level(spdlog::level::trace);
      m_AppLogger->set_level(spdlog::level::trace);
   }

   void Log::Shutdown()
   {
      m_EngineLogger->flush();
      m_AppLogger->flush();
   }

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
