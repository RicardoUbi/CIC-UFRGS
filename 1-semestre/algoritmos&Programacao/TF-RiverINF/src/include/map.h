#ifndef MAP_H
#define MAP_H

#include "defines.h" 

typedef struct {
    char data[MAP_MAX_ROWS][MAP_COLS]; // Array 2D para armazenar os dados do mapa
    int height;                        // Altura do mapa
    float scrollY;                     // Deslocamento vertical
    float scrollSpeed;                 // Velocidade de deslocamento
} Map;

void LoadMap(Map *map, const char *filename);
void LoadMapFromFile(Map *map, const char *filename);
void UpdateMapScroll(Map *map);
void DrawMap(const Map *map);
void DrawMapTiles(const Map *map);
int IsMapPositionSolid(const Map *map, float x, float y);
void GetMapIndices(const Map *map, float worldX, float worldY, int *outGridX, int *outGridY);
int ValidateMapFile(const char *filename);

#endif
