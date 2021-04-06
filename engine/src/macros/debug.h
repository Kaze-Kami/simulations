#pragma once

/*
 * Created by Kami-Kaze on 3/27/2021.
 *
 * See [definitions.h] for clarification about build types
 *
 * Debug macros
 *  - Evaluated on debug builds
 *  - Stripped on release builds
 *
 * DEBUG(x)         ;; evaluate [x]
 * SDEBUG(x)        ;; same as DEBUG(x) but scopes x (e.g. { x })
 */

#include "definitions.h"

#if defined(_BUILD_RELEASE)
#define DEBUG(x)
#define SDEBUG(x)
#else
#define DEBUG(x) x
#define SDEBUG(x) { x }
#endif
