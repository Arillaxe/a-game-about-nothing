#include "level.h"

void drawLevel()
{
  Level level = getGameState()->level;

  BeginMode2D(getGameState()->camera);

  Camera2D camera = getGameState()->camera;
  Vector2 topLeft = GetScreenToWorld2D((Vector2){0, 0}, camera);
  Vector2 bottomRight = GetScreenToWorld2D((Vector2){GetScreenWidth(), GetScreenHeight()}, camera);

  int startX = (int)(topLeft.x / TILE_SIZE) - 1;
  int endX = (int)(bottomRight.x / TILE_SIZE) + 1;
  int startY = (int)(topLeft.y / TILE_SIZE) - 1;
  int endY = (int)(bottomRight.y / TILE_SIZE) + 1;

  startX = fmax(0, startX);
  endX = fmin(WORLD_SIZE - 1, endX);
  startY = fmax(0, startY);
  endY = fmin(WORLD_SIZE - 1, endY);

  for (int i = 0; i < WORLD_SIZE; i++)
  {
    for (int j = 0; j < WORLD_SIZE; j++)
    {
      if (level.tiles[i][j] == TILE_ROCK)
      {
        DrawRectangle(i * TILE_SIZE, j * TILE_SIZE, TILE_SIZE, TILE_SIZE, BLACK);
      }
    }
  }

  EndMode2D();
}
