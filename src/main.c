#include <raylib.h>
#include "game_loader.h"
#include "globals.h"

int main()
{
  SetConfigFlags(FLAG_WINDOW_HIGHDPI);
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "FPS Test");
  // SetTargetFPS(0);

  loadGameLib();

  initGameStateFuncT initGameState = getInitGameStateFunc();

  if (!initGameState) {
    printf("Failed to load initGameState function\n");
    CloseWindow();
    return 1;
  }

  void *gameState = initGameState();

  while (!WindowShouldClose())
  {
    loadGameLib();

    tickFuncT gameTick = getGameTickFunc();

    if (gameTick)
    {
      gameTick(gameState);
    }
  }

  unloadGameLib();

  CloseWindow();

  return 0;
}
