#pragma once

#include <raylib.h>
#include "level.h"
#include "stdlib.h"
#include "level_generator.h"
#include "game.h"

typedef struct GameState
{
  Level level;
  Camera2D camera;
} GameState;

EXPORT GameState *initGameState();

GameState *getGameState();

void setGameState(GameState *state);
