#pragma once

#include <raylib.h>
#include <stdlib.h>
#include <stdbool.h>
#include "export.h"
#include "level.h" // Use existing TileType and TILE_SIZE

#define CHUNK_SIZE 16
#define CHUNK_PIXEL_SIZE (CHUNK_SIZE * TILE_SIZE)

// World wrapping - horizontal wrapping like a planet
#define WORLD_WIDTH_CHUNKS 64  // World is 64 chunks wide
#define WORLD_WIDTH_PIXELS (WORLD_WIDTH_CHUNKS * CHUNK_PIXEL_SIZE)

// Hash table for chunk storage
#define CHUNK_MAP_SIZE 256

typedef struct Chunk
{
  int x, y; // Chunk coordinates (not pixel coordinates)
  TileType tiles[CHUNK_SIZE][CHUNK_SIZE];
  bool generated;
  bool loaded;
} Chunk;

typedef struct ChunkNode
{
  Chunk chunk;
  struct ChunkNode* next;
} ChunkNode;

typedef struct ChunkMap
{
  ChunkNode* buckets[CHUNK_MAP_SIZE];
} ChunkMap;

// Chunk system functions
void initChunkSystem();
void destroyChunkSystem();

// Chunk management
Chunk* getChunk(int chunkX, int chunkY);
Chunk* createChunk(int chunkX, int chunkY);
void generateChunk(Chunk* chunk);
void loadChunksAroundPosition(Vector2 worldPos, int loadRadius);
void unloadDistantChunks(Vector2 worldPos, int unloadRadius);

// Utility functions with wrapping support
Vector2 worldToChunkCoord(Vector2 worldPos);
Vector2 chunkToWorldCoord(int chunkX, int chunkY);
TileType getTileAt(float worldX, float worldY);
int wrapChunkX(int chunkX); // Wrap chunk X coordinate
float wrapWorldX(float worldX); // Wrap world X coordinate

// Rendering
void drawChunks(Camera2D camera); 
