#ifndef MAP_H
#define MAP_H

#include "raylib.h"
#include <stdbool.h>

#define MAP_ROWS 20
#define MAP_COLS 24
#define TILE_SIZE 40

typedef struct {
    char data[MAP_ROWS][MAP_COLS];
} Map;

Map LoadMap(const char *filename);
void DrawMap(Map *map);
void UnloadMap(Map *map);

#endif
