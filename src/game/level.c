#include "level.h"
#include "game_state.h"
#include "chunk.h"

void drawLevel()
{
  // Use the new chunk-based rendering system
  drawChunks(getGameState()->camera);
}
