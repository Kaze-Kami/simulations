#pragma once

/*
 * Created by Kami-Kaze on 3/27/2021.
 *
 * Log macros for client side ('app')
 *
 * ToDo: Improve naming (app_log.h and engine_log.h)
 */

#include "logger.h"
#include "macros/definitions.h"

/*
 * App log macros
 */

#define LOG_TRACE(...)       ::Engine::Log::Logger::getAppLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)        ::Engine::Log::Logger::getAppLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)        ::Engine::Log::Logger::getAppLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)       ::Engine::Log::Logger::getAppLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...)    ::Engine::Log::Logger::getAppLogger()->critical(__VA_ARGS__)

/*
 * Stripped debug log macros
 */

#if defined(_BUILD_RELEASE)
#define LOG_DEBUG(...)
#else
#define LOG_DEBUG(...)          ::Engine::Log::Logger::getAppLogger()->debug(__VA_ARGS__)
#endif