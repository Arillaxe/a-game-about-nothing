#pragma once

#define WORLD_SIZE 256

#define TILE_SIZE 4

#include "raylib.h"
#include "math.h"

typedef enum TileType
{
  TILE_AIR,
  TILE_DIRT,
  TILE_ROCK,
  TILE_WATER,
  TILE_LAVA,
} TileType;

typedef struct Level
{
  TileType tiles[WORLD_SIZE][WORLD_SIZE];
} Level;

void drawLevel();
