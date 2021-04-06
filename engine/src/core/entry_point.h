#pragma once

/*
 * Created by Kami-Kaze on 3/27/2021.
 *
 * Entry point for client side
 */

#include "core/logging/logger.h"
#include "core/logging/engine_log.h"

#include "core/application/application.h"

namespace Engine {

    inline int main() {
        // Initialize loggers
        Log::Logger::init();

        // ToDo: Log Info
        LOG_CORE_INFO("ToDo: Log Engine Info");

        // Create application
        Application* application = Application::createApplication();
        application->init();
        application->run();

        // Clean up
        delete application;
        return 0;
    }

}