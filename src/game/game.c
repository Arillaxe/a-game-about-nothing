#include "game.h"
#include "chunk.h"

static int frameCounter = 0; // For periodic cleanup

EXPORT void gameTick(GameState *gameState)
{
  setGameState(gameState);

  // Basic camera movement with arrow keys
  Vector2 movement = {0};
  if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) movement.x = -200.0f;
  if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) movement.x = 200.0f;
  if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) movement.y = -200.0f;
  if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) movement.y = 200.0f;
  
  movement.x *= GetFrameTime();
  movement.y *= GetFrameTime();
  
  gameState->playerPos.x += movement.x;
  gameState->playerPos.y += movement.y;
  
  // Wrap player position horizontally for seamless world
  gameState->playerPos.x = wrapWorldX(gameState->playerPos.x);
  
  // Center camera on player
  gameState->camera.target = gameState->playerPos;
  
  // Periodic cleanup of distant chunks (every 60 frames = ~1 second)
  frameCounter++;
  if (frameCounter >= 60) {
    unloadDistantChunks(gameState->playerPos, 8);
    frameCounter = 0;
  }

  BeginDrawing();
  ClearBackground(SKYBLUE);

  // Draw the infinite world using chunks (chunks auto-load as needed)
  drawChunks(gameState->camera);
  
  // Draw player
  BeginMode2D(gameState->camera);
  DrawCircle((int)gameState->playerPos.x, (int)gameState->playerPos.y, 8, RED);
  EndMode2D();

  DrawFPS(10, 10);
  DrawText("Use WASD/Arrow keys to explore the wrapping world!", 10, 30, 20, WHITE);
  
  Vector2 chunkCoord = worldToChunkCoord(gameState->playerPos);
  DrawText(TextFormat("Player: (%.0f, %.0f) Chunk: (%.0f, %.0f) WorldWidth: %d", 
                     gameState->playerPos.x, gameState->playerPos.y,
                     chunkCoord.x, chunkCoord.y, WORLD_WIDTH_PIXELS), 10, 55, 16, WHITE);

  drawUI();

  EndDrawing();
}
