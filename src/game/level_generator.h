#pragma once

#include "level.h"
#include "stb_perlin.h"

Level generateLevel();

void generate_surface(Level *level);

void generate_caves(Level *level);

void generate_cave_details(Level *level);

void smooth_caves(Level *level);
