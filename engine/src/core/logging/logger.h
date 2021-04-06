#pragma once

/*
 * Created by Kami-Kaze on 3/27/2021.
 */

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace Engine::Log {

    class Logger {
    public:
        static void init();

        static std::shared_ptr<spdlog::logger>& getEngineLogger();
        static std::shared_ptr<spdlog::logger>& getAppLogger();

    private:
        static std::shared_ptr<spdlog::logger> engineLogger, appLogger;
        static bool initialized;

    };

}
