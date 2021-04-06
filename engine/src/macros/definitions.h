#pragma once

/*
 * Created by Kami-Kaze on 3/27/2021.
 *
 * A build is considered a 'release build' if NDEBUG is defined.
 * Everything else is considered a 'debug build'
 *
 * Global preprocessor definitions:
 *
 * _BUILD_RELEASE           ;; set for release builds
 */


// Release build
#if defined(NDEBUG)
#define _BUILD_RELEASE
#endif