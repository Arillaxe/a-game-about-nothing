#include "game_state.h"
#include "chunk.h"

static GameState *gameState = NULL;

EXPORT GameState *initGameState()
{
  gameState = malloc(sizeof(GameState));

  Camera2D camera = {0};
  camera.zoom = 1.0f;
  camera.offset = (Vector2){400.0f, 200.0f}; // Center on screen
  
  Vector2 playerPos = {0.0f, 100.0f}; // Start player at surface

  (*gameState) = (GameState){
      .camera = camera,
      .playerPos = playerPos,
  };

  // Initialize the chunk system
  initChunkSystem();
  
  // Load initial chunks around player spawn
  loadChunksAroundPosition(playerPos, 3);

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
