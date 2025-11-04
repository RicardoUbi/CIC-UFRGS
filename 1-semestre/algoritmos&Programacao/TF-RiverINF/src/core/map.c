#include "map.h"
#include <stdio.h>

Map LoadMap(const char *filename)
{
    Map map = {0};
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erro ao carregar mapa: %s\n", filename);
        return map;
    }

    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            int c = fgetc(file);
            if (c == EOF || c == '\n') c = ' '; // espaço = água
            map.data[row][col] = (char)c;
        }
    }

    fclose(file);
    return map;
}

void DrawMap(Map *map)
{
    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            char cell = map->data[row][col];
            int x = col * TILE_SIZE;
            int y = row * TILE_SIZE;

            switch (cell) {
                case 'T': DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, GREEN); break;
                case 'N': DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, RED); break;
                case 'X': DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, MAROON); break;
                case 'G': DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, YELLOW); break;
                case 'P': DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, GRAY); break;
                default:  DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, BLUE); break;
            }
        }
    }
}

void UnloadMap(Map *map) {
    // Nada a liberar ainda, mas evita erro de linkagem
}