#include "game_state.h"

static GameState *gameState = NULL;

EXPORT GameState *initGameState()
{
  gameState = malloc(sizeof(GameState));

  Camera2D camera = {0};
  camera.zoom = 1.0f;
  camera.offset = (Vector2){0.0f, 0.0f};

  (*gameState) = (GameState){
      .camera = camera,
      .level = generateLevel(),
  };

  return gameState;
}

GameState *getGameState()
{
  return gameState;
}

void setGameState(GameState *state)
{
  gameState = state;
}
