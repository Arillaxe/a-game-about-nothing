#include "chunk.h"
#include "level_generator.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

#ifndef PI
#define PI 3.14159265358979323846f
#endif

static ChunkMap chunkMap = {0};

// Hash function for chunk coordinates
static unsigned int hashChunkCoord(int x, int y) {
    unsigned int hash = 0;
    hash = ((unsigned int)x * 73856093) ^ ((unsigned int)y * 19349663);
    return hash % CHUNK_MAP_SIZE;
}

// Wrap chunk X coordinate to create seamless world
int wrapChunkX(int chunkX) {
    // Use proper modulo that handles negative numbers correctly
    chunkX = chunkX % WORLD_WIDTH_CHUNKS;
    if (chunkX < 0) chunkX += WORLD_WIDTH_CHUNKS;
    return chunkX;
}

// Wrap world X coordinate
float wrapWorldX(float worldX) {
    // Use proper fmod that handles negative numbers correctly
    while (worldX < 0) worldX += WORLD_WIDTH_PIXELS;
    while (worldX >= WORLD_WIDTH_PIXELS) worldX -= WORLD_WIDTH_PIXELS;
    return worldX;
}

void initChunkSystem() {
    memset(&chunkMap, 0, sizeof(ChunkMap));
}

void destroyChunkSystem() {
    for (int i = 0; i < CHUNK_MAP_SIZE; i++) {
        ChunkNode* node = chunkMap.buckets[i];
        while (node) {
            ChunkNode* next = node->next;
            free(node);
            node = next;
        }
        chunkMap.buckets[i] = NULL;
    }
}

Chunk* getChunk(int chunkX, int chunkY) {
    // Wrap the X coordinate for seamless world
    chunkX = wrapChunkX(chunkX);
    
    unsigned int hash = hashChunkCoord(chunkX, chunkY);
    ChunkNode* node = chunkMap.buckets[hash];
    
    // Search for existing chunk
    while (node) {
        if (node->chunk.x == chunkX && node->chunk.y == chunkY) {
            return &node->chunk;
        }
        node = node->next;
    }
    
    // Don't create chunks during rendering - return NULL to avoid frame drops
    return NULL;
}

Chunk* createChunk(int chunkX, int chunkY) {
    // Wrap the X coordinate for seamless world
    chunkX = wrapChunkX(chunkX);
    
    // Check if chunk already exists after wrapping
    Chunk* existing = getChunk(chunkX, chunkY);
    if (existing) return existing;
    
    // Create new chunk (separate function for controlled creation)
    ChunkNode* newNode = malloc(sizeof(ChunkNode));
    if (!newNode) return NULL;
    
    newNode->chunk.x = chunkX;
    newNode->chunk.y = chunkY;
    newNode->chunk.generated = false;
    newNode->chunk.loaded = true;
    memset(newNode->chunk.tiles, TILE_AIR, sizeof(newNode->chunk.tiles));
    
    // Insert at head of bucket
    unsigned int hash = hashChunkCoord(chunkX, chunkY);
    newNode->next = chunkMap.buckets[hash];
    chunkMap.buckets[hash] = newNode;
    
    // Generate the chunk
    generateChunk(&newNode->chunk);
    
    return &newNode->chunk;
}

void generateChunk(Chunk* chunk) {
    if (chunk->generated) return;
    
    // Convert chunk coordinates to world coordinates
    int worldStartX = chunk->x * CHUNK_SIZE;
    int worldStartY = chunk->y * CHUNK_SIZE;
    
    // For seamless wrapping, we need to ensure noise is periodic
    // Use wrapped coordinates that ensure continuity at boundaries
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            int localWorldX = worldStartX + x;
            int worldY = worldStartY + y;
            
            // Create seamless noise by using wrapped coordinates
            // Map world coordinates to 0-1 range for periodic noise
            float normalizedX = (float)localWorldX / WORLD_WIDTH_PIXELS;
            float noiseX = cos(normalizedX * 2 * PI); // Convert to periodic
            float noiseX2 = sin(normalizedX * 2 * PI);
            
            // Surface generation (seamless across world boundaries)
            float height = stb_perlin_noise3(noiseX * 4, noiseX2 * 4, 0, 0, 0, 0) * 30.0f;
            int surface_y = (int)(128 + height);
            
            if (worldY >= surface_y) {
                chunk->tiles[x][y] = (worldY < surface_y + 3) ? TILE_DIRT : TILE_ROCK;
            } else {
                chunk->tiles[x][y] = TILE_AIR;
            }
            
            // Simplified cave generation (also seamless)
            if (chunk->tiles[x][y] != TILE_AIR && worldY > 140) {
                float cave_noise = stb_perlin_noise3(noiseX * 8, worldY * 0.02f, noiseX2 * 8, 0, 0, 0);
                if (cave_noise > 0.3f) {
                    chunk->tiles[x][y] = TILE_AIR;
                }
            }
            
            // Add some water in very deep areas
            if (chunk->tiles[x][y] == TILE_AIR && worldY > 200) {
                float water_noise = stb_perlin_noise3(noiseX * 12, worldY * 0.05f, noiseX2 * 12, 0, 0, 0);
                if (water_noise > 0.6f) {
                    chunk->tiles[x][y] = TILE_WATER;
                }
            }
        }
    }
    
    chunk->generated = true;
}

Vector2 worldToChunkCoord(Vector2 worldPos) {
    // Wrap X coordinate, keep Y infinite
    float wrappedX = wrapWorldX(worldPos.x);
    return (Vector2){
        floorf(wrappedX / CHUNK_PIXEL_SIZE),
        floorf(worldPos.y / CHUNK_PIXEL_SIZE)
    };
}

Vector2 chunkToWorldCoord(int chunkX, int chunkY) {
    return (Vector2){
        wrapChunkX(chunkX) * CHUNK_PIXEL_SIZE,
        chunkY * CHUNK_PIXEL_SIZE
    };
}

TileType getTileAt(float worldX, float worldY) {
    // Wrap X coordinate
    float wrappedX = wrapWorldX(worldX);
    
    int chunkX = (int)floorf(wrappedX / CHUNK_PIXEL_SIZE);
    int chunkY = (int)floorf(worldY / CHUNK_PIXEL_SIZE);
    
    Chunk* chunk = getChunk(chunkX, chunkY);
    if (!chunk) return TILE_AIR;
    
    int tileX = (int)floorf(wrappedX / TILE_SIZE) % CHUNK_SIZE;
    int tileY = (int)floorf(worldY / TILE_SIZE) % CHUNK_SIZE;
    
    if (tileX < 0) tileX += CHUNK_SIZE;
    if (tileY < 0) tileY += CHUNK_SIZE;
    
    return chunk->tiles[tileX][tileY];
}

void loadChunksAroundPosition(Vector2 worldPos, int loadRadius) {
    Vector2 centerChunk = worldToChunkCoord(worldPos);
    
    for (int dx = -loadRadius; dx <= loadRadius; dx++) {
        for (int dy = -loadRadius; dy <= loadRadius; dy++) {
            int chunkX = (int)centerChunk.x + dx;
            int chunkY = (int)centerChunk.y + dy;
            
            // Check if chunk already exists before creating
            Chunk* existing = getChunk(chunkX, chunkY);
            if (!existing) {
                createChunk(chunkX, chunkY);
            }
        }
    }
}

void unloadDistantChunks(Vector2 worldPos, int unloadRadius) {
    Vector2 centerChunk = worldToChunkCoord(worldPos);
    
    for (int i = 0; i < CHUNK_MAP_SIZE; i++) {
        ChunkNode** nodePtr = &chunkMap.buckets[i];
        
        while (*nodePtr) {
            ChunkNode* node = *nodePtr;
            
            // Calculate distance considering wrapping
            int dx = abs(node->chunk.x - (int)centerChunk.x);
            int dy = abs(node->chunk.y - (int)centerChunk.y);
            
            // Handle wrapping distance calculation
            int wrapDx = WORLD_WIDTH_CHUNKS - dx;
            if (wrapDx < dx) dx = wrapDx;
            
            if (dx > unloadRadius || dy > unloadRadius) {
                // Unload this chunk
                *nodePtr = node->next;
                free(node);
            } else {
                nodePtr = &node->next;
            }
        }
    }
}

void drawChunks(Camera2D camera) {
    // Calculate which chunks are visible
    Vector2 screenSize = {GetScreenWidth(), GetScreenHeight()};
    Vector2 topLeft = GetScreenToWorld2D((Vector2){0, 0}, camera);
    Vector2 bottomRight = GetScreenToWorld2D(screenSize, camera);
    
    // Calculate chunk ranges - handle wrapping by using unwrapped coordinates
    int startChunkX = (int)floorf(topLeft.x / CHUNK_PIXEL_SIZE) - 1;
    int endChunkX = (int)floorf(bottomRight.x / CHUNK_PIXEL_SIZE) + 1;
    int startChunkY = (int)floorf(topLeft.y / CHUNK_PIXEL_SIZE) - 1;
    int endChunkY = (int)floorf(bottomRight.y / CHUNK_PIXEL_SIZE) + 1;
    
    BeginMode2D(camera);
    
    // Draw visible chunks - iterate through actual coordinates, not wrapped ones
    for (int chunkX = startChunkX; chunkX <= endChunkX; chunkX++) {
        for (int chunkY = startChunkY; chunkY <= endChunkY; chunkY++) {
            // Get/create chunk using wrapped coordinates for storage
            int wrappedChunkX = wrapChunkX(chunkX);
            
            // Look up existing chunk
            unsigned int hash = hashChunkCoord(wrappedChunkX, chunkY);
            ChunkNode* node = chunkMap.buckets[hash];
            
            Chunk* chunk = NULL;
            while (node) {
                if (node->chunk.x == wrappedChunkX && node->chunk.y == chunkY) {
                    chunk = &node->chunk;
                    break;
                }
                node = node->next;
            }
            
            // If chunk doesn't exist, create it with wrapped coordinates
            if (!chunk) {
                ChunkNode* newNode = malloc(sizeof(ChunkNode));
                if (!newNode) continue;
                
                newNode->chunk.x = wrappedChunkX;
                newNode->chunk.y = chunkY;
                newNode->chunk.generated = false;
                newNode->chunk.loaded = true;
                memset(newNode->chunk.tiles, TILE_AIR, sizeof(newNode->chunk.tiles));
                
                // Insert at head of bucket
                newNode->next = chunkMap.buckets[hash];
                chunkMap.buckets[hash] = newNode;
                
                chunk = &newNode->chunk;
                generateChunk(chunk);
            }
            
            if (!chunk || !chunk->generated) continue;
            
            // Draw tiles using the ORIGINAL (unwrapped) chunk coordinates for positioning
            for (int x = 0; x < CHUNK_SIZE; x++) {
                for (int y = 0; y < CHUNK_SIZE; y++) {
                    if (chunk->tiles[x][y] == TILE_AIR) continue;
                    
                    // Use original chunkX for positioning - this allows wrapping display
                    int worldX = chunkX * CHUNK_PIXEL_SIZE + x * TILE_SIZE;
                    int worldY = chunkY * CHUNK_PIXEL_SIZE + y * TILE_SIZE;
                    
                    Color color = GRAY;
                    switch (chunk->tiles[x][y]) {
                        case TILE_DIRT: color = BROWN; break;
                        case TILE_ROCK: color = GRAY; break;
                        case TILE_WATER: color = BLUE; break;
                        case TILE_LAVA: color = ORANGE; break;
                        default: break;
                    }
                    
                    DrawRectangle(worldX, worldY, TILE_SIZE, TILE_SIZE, color);
                }
            }
        }
    }
    
    EndMode2D();
} 
