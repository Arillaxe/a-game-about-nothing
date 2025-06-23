#pragma once

// Cross-platform export macro for shared library functions
#ifndef EXPORT
#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_MAC
#define EXPORT
#endif
#elif defined _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif
#endif 