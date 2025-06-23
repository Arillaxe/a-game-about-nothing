#include "level_generator.h"

Level generateLevel()
{
  Level level = {0};

  for (int i = 0; i < WORLD_SIZE; i++)
  {
    for (int j = 0; j < WORLD_SIZE; j++)
    {
      if (j > 32)
      {
        level.tiles[i][j] = TILE_ROCK;
      }
      else
      {
        level.tiles[i][j] = TILE_AIR;
      }
    }
  }

  return level;
}
