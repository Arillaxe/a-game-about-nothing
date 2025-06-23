#pragma once

#include <raylib.h>
#include "stdlib.h"
#include "export.h"

// Forward declaration to avoid circular dependency
struct GameState;

typedef struct GameState
{
  Camera2D camera;
  Vector2 playerPos; // Track player position for chunk loading
} GameState;

EXPORT GameState *initGameState();

GameState *getGameState();

void setGameState(GameState *state);
