#pragma once

/*
 * Created by Kami-Kaze on 3/27/2021.
 *
 * See [definitions.h] for clarification about build types
 *
 * Assert macros
 *  - Evaluated evaluated on debug builds
 *  - Stripped on release builds unless _ASSERTS_ENABLE is defined
 *
 */

#include "definitions.h"

#if defined(_ASSERTS_ENABLE) || !defined(_BUILD_RELEASE)
#include "core/logging/logger.h"
#define CORE_ASSERT(x, m) { if (!(x)) {\
::Engine::Log::Logger::getEngineLogger()->error("Assertion failed: {} in {}:{}", m, __FILE__, __LINE__); \
}}
#define ASSERT(x, m) { if (!(x)) {\
::Engine::Log::Logger::getAppLogger()->error("Assertion failed: {} in {}:{}", m, __FILE__, __LINE__); \
}}
#else
#define CORE_ASSERT(x, m)
#define ASSERT(x, m)
#endif

