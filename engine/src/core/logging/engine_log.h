#pragma once

/*
 * Created by Kami-Kaze on 3/27/2021.
 *
 * Log macros for engine side
 *
 * ToDo: Improve naming (app_log.h and engine_log.h)
 */

#include "logger.h"
#include "macros/definitions.h"

/*
 * Engine log macros
 */

#define LOG_CORE_TRACE(...)       ::Engine::Log::Logger::getEngineLogger()->trace(__VA_ARGS__)
#define LOG_CORE_INFO(...)        ::Engine::Log::Logger::getEngineLogger()->info(__VA_ARGS__)
#define LOG_CORE_WARN(...)        ::Engine::Log::Logger::getEngineLogger()->warn(__VA_ARGS__)
#define LOG_CORE_ERROR(...)       ::Engine::Log::Logger::getEngineLogger()->error(__VA_ARGS__)
#define LOG_CORE_CRITICAL(...)    ::Engine::Log::Logger::getEngineLogger()->critical(__VA_ARGS__)

/*
 * Stripped debug log macros
 */

#if defined(_BUILD_RELEASE)
#define LOG_CORE_DEBUG(...)
#else
#define LOG_CORE_DEBUG(...)       ::Engine::Log::Logger::getEngineLogger()->debug(__VA_ARGS__)
#endif