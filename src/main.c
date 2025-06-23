#include <raylib.h>
#include "game_loader.h"
#include "globals.h"

int main()
{
  SetConfigFlags(FLAG_WINDOW_HIGHDPI);
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "FPS Test");
  SetTargetFPS(60);

  loadGameLib();

  initGameStateFuncT initGameState = getInitGameStateFunc();

  void *gameState = initGameState();

  while (!WindowShouldClose())
  {
    loadGameLib();

    tickFuncT gameTick = getGameTickFunc();

    if (gameTick)
    {
      gameTick(&gameState);
    }
  }

  unloadGameLib();

  CloseWindow();

  return 0;
}
