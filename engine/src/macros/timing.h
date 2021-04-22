#pragma once

/*
 * Created by Kami-Kaze on 4/13/2021.
 */

#include <chrono>
#include "core/logging/engine_log.h"

static void logTiming(double time, const char* name) {
    LOG_CORE_INFO("{} executed in {:.4f} ms", name, time * 1e3);
}

#define TIME_IT(x, name) {\
auto start = std::chrono::high_resolution_clock::now(); \
x; \
auto end = std::chrono::high_resolution_clock::now(); \
std::chrono::duration<double> elapsed = end - start;    \
logTiming(elapsed.count(), name); \
}
