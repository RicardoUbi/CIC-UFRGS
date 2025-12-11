#include "map.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void LoadMap(Map *map, const char *filename)
{
    map->height = 0;
    map->scrollY = 0.0f;
    map->scrollSpeed = 2.0f;

    FILE *f = fopen(filename, "r");
    if (!f)
    {
        //printf("Erro ao abrir %s. Criando mapa vazio.\n", filename);
        for (int y = 0; y < MAP_ROWS; y++)
        {
            for (int x = 0; x < MAP_COLS; x++)
            {
                map->data[y][x] = ' ';
            }
        }
        map->height = MAP_ROWS;
        return;
    }

    char line[256];
    int lineCount = 0;

    while (fgets(line, sizeof(line), f) && lineCount < MAP_ROWS)
    {
        size_t len = strlen(line);

        if (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
        {
            line[len - 1] = '\0';
            len--;
        }

        for (int x = 0; x < MAP_COLS; x++)
        {
            if (x < len && line[x] != '\0')
            {
                map->data[lineCount][x] = line[x];
            }
            else
            {
                map->data[lineCount][x] = ' ';
            }
        }
        lineCount++;
    }
    map->height = lineCount;
    fclose(f);
    //printf("Mapa '%s' carregado: %d linhas\n", filename, map->height);
}

void UpdateMapScroll(Map *map)
{
    if (!map) return;

    map->scrollY -= map->scrollSpeed; // ajustei aqui, pois estava + e estava dando ré no mapa

    float maxScrollN = -(map->height * TILE_SIZE);

    if (map->scrollY < maxScrollN)
        map->scrollY = maxScrollN;
}


void DrawMap(const Map *map)
{
    int startY = (int)(map->scrollY / TILE_SIZE);
    int endY = startY + (SCREEN_HEIGHT / TILE_SIZE) + 1;

    if (endY > map->height)
        endY = map->height;

    for (int y = startY; y < endY; y++)
    {
        for (int x = 0; x < MAP_COLS; x++)
        {
            char tile = map->data[y][x];

            if (tile == 'T')
            {
                float screenY = (y * TILE_SIZE) - map->scrollY;
                DrawRectangle(x * TILE_SIZE, screenY,
                              TILE_SIZE, TILE_SIZE, GREEN);
            }
        }
    }
}

int IsMapPositionSolid(const Map *map, int gridX, int gridY)
{
    // Converte coordenadas de mundo para grid
    if (gridX < 0 || gridX >= MAP_COLS || gridY < 0 || gridY >= map->height)
    {
        return 0; // Fora do mapa não é sólido
    }

    char tile = map->data[gridY][gridX];

    return tile == 'T';
}

void GetMapIndices(const Map *map, float worldX, float worldY, int *outGridX, int *outGridY)
{
    *outGridX = (int)(worldX / TILE_SIZE);

    float worldYInMap = worldY + map->scrollY;
    *outGridY = (int)(worldYInMap / TILE_SIZE);
}

int ValidateMapFile(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        return 0;

    char line[256];
    int lineCount = 0;
    int valid = 1;

    while (fgets(line, sizeof(line), f) && lineCount < MAP_ROWS)
    {
        size_t len = strlen(line);
        if (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
        {
            line[len - 1] = '\0';
            len--;
        }

        if (len != MAP_COLS)
        {
            valid = 0;
            break;
        }
        for (int i = 0; i < 5; i++)
        {
            if (line[i] != 'T' || line[MAP_COLS - 1 - i] != 'T')
            {
                valid = 0;
                break;
            }
        }

        if (!valid)
        {
            break;
        }
        lineCount++;
    }

    fclose(f);

    if (lineCount < 20)
        valid = 0;

    return valid;
}
