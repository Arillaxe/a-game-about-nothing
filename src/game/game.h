#pragma once

#include <raylib.h>
#include "stdio.h"

#ifdef __APPLE__
#include <TargetConditionals.h>
#if TARGET_OS_MAC
#define EXPORT
#endif
#elif defined _WIN32
#define EXPORT __declspec(dllexport)
#endif

#include "ui.h"

#include "level.h"
#include "game_state.h"

void gameTick(GameState *gameState);
