#include "game.h"

EXPORT void gameTick(GameState *gameState)
{
  setGameState(gameState);

  BeginDrawing();
  ClearBackground(RAYWHITE);

  drawLevel();

  DrawFPS(10, 10);

  DrawText("Yo wazzup", 10, 30, 24, RED);

  drawUI();

  EndDrawing();
}
