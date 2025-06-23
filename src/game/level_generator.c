#include "level_generator.h"

Level generateLevel()
{
  Level level = {0};

  generate_surface(&level);
  generate_caves(&level);
  generate_cave_details(&level);

  return level;
}

void generate_surface(Level *level)
{
  for (int x = 0; x < WORLD_SIZE; x++)
  {
    float height = 0;
    for (int octave = 0; octave < 3; octave++)
    {
      float freq = 0.01f * pow(2, octave);
      float amp = 50.0f * pow(0.5, octave);
      height += stb_perlin_noise3(x * freq, 0, 0, 0, 0, 0) * amp;
    }

    int surface_y = (int)(WORLD_SIZE * 0.5 + height);

    // Fill from surface down
    for (int y = surface_y; y < WORLD_SIZE; y++)
    {
      level->tiles[x][y] = (y < surface_y + 5) ? TILE_DIRT : TILE_ROCK;
    }
  }
}

void generate_caves(Level *level)
{
  // Multi-octave cave generation for more complex cave systems
  for (int x = 0; x < WORLD_SIZE; x++)
  {
    for (int y = 0; y < WORLD_SIZE; y++)
    {
      if (level->tiles[x][y] == TILE_AIR)
        continue;

      // Skip surface layers to avoid surface caves
      if (y < WORLD_SIZE * 0.5 + 20)
        continue;

      float cave_value = 0;

      // Large cave chambers
      cave_value += stb_perlin_noise3(x * 0.015f, y * 0.015f, 0, 0, 0, 0) * 0.6f;

      // Medium cave tunnels
      cave_value += stb_perlin_noise3(x * 0.03f, y * 0.03f, 0, 0, 0, 0) * 0.3f;

      // Small cave details
      cave_value += stb_perlin_noise3(x * 0.06f, y * 0.06f, 0, 0, 0, 0) * 0.1f;

      // Depth-based cave probability (more caves deeper)
      float depth_factor = (y - WORLD_SIZE * 0.5) / (WORLD_SIZE * 0.5);
      depth_factor = fmax(0, depth_factor);

      // Adjust threshold based on depth
      float threshold = 0.4f - (depth_factor * 0.2f);

      if (cave_value > threshold)
      {
        level->tiles[x][y] = TILE_AIR;
      }
    }
  }
}

void generate_cave_details(Level *level)
{
  // Add stalactites and stalagmites
  for (int x = 1; x < WORLD_SIZE - 1; x++)
  {
    for (int y = 1; y < WORLD_SIZE - 1; y++)
    {
      if (level->tiles[x][y] != TILE_AIR)
        continue;

      // Check for cave ceiling (rock above, air below)
      if (level->tiles[x][y - 1] == TILE_ROCK && level->tiles[x][y + 1] == TILE_AIR)
      {
        float stalactite_noise = stb_perlin_noise3(x * 0.1f, y * 0.1f, 0, 0, 0, 0);
        if (stalactite_noise > 0.6f)
        {
          // Create stalactite hanging down
          int length = (int)(stalactite_noise * 4) + 1;
          for (int i = 0; i < length && y + i < WORLD_SIZE; i++)
          {
            if (level->tiles[x][y + i] == TILE_AIR)
              level->tiles[x][y + i] = TILE_ROCK;
            else
              break;
          }
        }
      }

      // Check for cave floor (air above, rock below)
      if (level->tiles[x][y - 1] == TILE_AIR && level->tiles[x][y + 1] == TILE_ROCK)
      {
        float stalagmite_noise = stb_perlin_noise3(x * 0.1f + 1000, y * 0.1f + 1000, 0, 0, 0, 0);
        if (stalagmite_noise > 0.6f)
        {
          // Create stalagmite growing up
          int length = (int)(stalagmite_noise * 3) + 1;
          for (int i = 0; i < length && y - i >= 0; i++)
          {
            if (level->tiles[x][y - i] == TILE_AIR)
              level->tiles[x][y - i] = TILE_ROCK;
            else
              break;
          }
        }
      }
    }
  }

  // Add underground water/lava pools
  for (int x = 0; x < WORLD_SIZE; x++)
  {
    for (int y = WORLD_SIZE * 0.8; y < WORLD_SIZE; y++)
    {
      if (level->tiles[x][y] != TILE_AIR)
        continue;

      // Check if this is a low point in a cave
      bool is_bottom = true;
      for (int dx = -2; dx <= 2; dx++)
      {
        if (x + dx >= 0 && x + dx < WORLD_SIZE && y + 1 < WORLD_SIZE)
        {
          if (level->tiles[x + dx][y + 1] == TILE_AIR)
          {
            is_bottom = false;
            break;
          }
        }
      }

      if (is_bottom)
      {
        float water_noise = stb_perlin_noise3(x * 0.05f, y * 0.05f, 0, 0, 0, 0);
        if (water_noise > 0.4f)
        {
          // Very deep = lava, otherwise water
          TileType liquid = (y > WORLD_SIZE * 0.9) ? TILE_LAVA : TILE_WATER;
          level->tiles[x][y] = liquid;

          // Spread liquid horizontally
          for (int dx = -1; dx <= 1; dx++)
          {
            if (x + dx >= 0 && x + dx < WORLD_SIZE)
            {
              if (level->tiles[x + dx][y] == TILE_AIR)
              {
                float spread_chance = stb_perlin_noise3((x + dx) * 0.1f, y * 0.1f, 0, 0, 0, 0);
                if (spread_chance > 0.3f)
                  level->tiles[x + dx][y] = liquid;
              }
            }
          }
        }
      }
    }
  }

  // Clean up isolated single blocks and smooth cave walls
  smooth_caves(level);
}

void smooth_caves(Level *level)
{
  // Two-pass smoothing to make caves look more natural
  for (int pass = 0; pass < 2; pass++)
  {
    Level temp_level = *level;

    for (int x = 1; x < WORLD_SIZE - 1; x++)
    {
      for (int y = 1; y < WORLD_SIZE - 1; y++)
      {
        if (temp_level.tiles[x][y] == TILE_ROCK)
        {
          // Count surrounding air blocks
          int air_count = 0;
          for (int dx = -1; dx <= 1; dx++)
          {
            for (int dy = -1; dy <= 1; dy++)
            {
              if (dx == 0 && dy == 0)
                continue;
              if (temp_level.tiles[x + dx][y + dy] == TILE_AIR)
                air_count++;
            }
          }

          // Remove isolated rock blocks
          if (air_count >= 6)
          {
            level->tiles[x][y] = TILE_AIR;
          }
        }
        else if (temp_level.tiles[x][y] == TILE_AIR)
        {
          // Fill tiny air pockets
          int rock_count = 0;
          for (int dx = -1; dx <= 1; dx++)
          {
            for (int dy = -1; dy <= 1; dy++)
            {
              if (dx == 0 && dy == 0)
                continue;
              if (temp_level.tiles[x + dx][y + dy] == TILE_ROCK ||
                  temp_level.tiles[x + dx][y + dy] == TILE_DIRT)
                rock_count++;
            }
          }

          if (rock_count >= 7)
          {
            level->tiles[x][y] = TILE_ROCK;
          }
        }
      }
    }
  }
}
