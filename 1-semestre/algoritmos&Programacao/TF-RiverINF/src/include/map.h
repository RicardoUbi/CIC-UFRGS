#ifndef MAP_H
#define MAP_H

#include "raylib.h"
#include <stdbool.h>
#include "defines.h" 

typedef struct {
    char data[MAP_ROWS][MAP_COLS];
} Map;

Map LoadMap(const char *filename);
void DrawMap(Map *map, float offsetY);
void UnloadMap(Map *map);

#endif
