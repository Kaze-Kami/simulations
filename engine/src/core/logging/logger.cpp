/*
 * Created by Kami-Kaze on 3/27/2021.
 */

#include "logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Engine::Log {
    std::shared_ptr<spdlog::logger> Logger::engineLogger, Logger::appLogger;

    bool Logger::initialized = false;

    void Logger::init() {
        if (initialized) {
            engineLogger->warn("Repeated to Logger::init()! (Ignored)");
            return;
        }

        // spdlog::set_pattern("%^[%T | %l] %v%$");

        // create engine logger
        engineLogger = spdlog::stdout_color_mt("engine");
        engineLogger->set_pattern("%^%T [%n | %l] %v%$");
        engineLogger->set_level(spdlog::level::trace);

        // create app logger
        appLogger = spdlog::stdout_color_mt("app");
        appLogger->set_pattern("%^%T [%l] %v%$");
        appLogger->set_level(spdlog::level::trace);

        // initialisation complete
        engineLogger->info("Logger initialized.");
        initialized = true;
    }

    std::shared_ptr<spdlog::logger> &Logger::getEngineLogger() { return engineLogger; }
    std::shared_ptr<spdlog::logger> &Logger::getAppLogger() { return appLogger; }
}