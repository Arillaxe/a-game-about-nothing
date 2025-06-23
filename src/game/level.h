#pragma once

#define WORLD_SIZE 64

#define TILE_SIZE 16

#include "raylib.h"
#include "game_state.h"
#include "math.h"

typedef enum TileType
{
  TILE_AIR,
  TILE_ROCK,
} TileType;

typedef struct Level
{
  TileType tiles[WORLD_SIZE][WORLD_SIZE];
} Level;

void drawLevel();
