#include "map.h"
#include <stdio.h>
#include <string.h>
#include "game.h"

Map LoadMap(const char *filename)
{
    Map map = {0};
    FILE *file = fopen(filename, "r");

    if (!file) {
        printf("Erro ao carregar mapa: %s. Gerando mapa padrão...\n", filename);
        for (int row = 0; row < MAP_ROWS; row++) {
            for (int col = 0; col < MAP_COLS; col++) {
                if (col < 5 || col >= MAP_COLS - 5)
                    map.data[row][col] = 'T';
                else
                    map.data[row][col] = ' ';
            }
        }
        return map;
    }

    char line[256];
    int row = 0;
    while (row < MAP_ROWS && fgets(line, sizeof(line), file)) {
        int len = strlen(line);
        for (int col = 0; col < MAP_COLS; col++) {
            if (col < len && line[col] != '\n' && line[col] != '\r')
                map.data[row][col] = line[col];
            else
                map.data[row][col] = ' '; // completa linha curta com rio
        }
        row++;
    }

    // Caso o arquivo tenha menos linhas que MAP_ROWS
    for (; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            map.data[row][col] = ' ';
        }
    }

    fclose(file);
    printf("Mapa carregado com sucesso: %s\n", filename);
    return map;
}


void DrawMap(Map *map, float cameraY)
{
    // Calcular quais linhas do mapa estão visíveis
    int startRow = (int)(cameraY / TILE_SIZE);
    int endRow = startRow + (SCREEN_HEIGHT / TILE_SIZE) + 2;
    
    for (int row = startRow; row < endRow && row < MAP_ROWS; row++)
    {
        for (int col = 0; col < MAP_COLS; col++)
        {
            char cell = map->data[row][col];
            int x = col * TILE_SIZE;
            int y = row * TILE_SIZE - cameraY;

            // Só desenhar se estiver dentro da tela
            if (y >= -TILE_SIZE && y < SCREEN_HEIGHT)
            {
                switch (cell)
                {
                    case 'T': 
                        DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, GREEN); 
                        break;
                    case 'N': 
                        DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, RED); 
                        break;
                    case 'X': 
                        DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, MAROON); 
                        break;
                    case 'G': 
                        DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, YELLOW); 
                        break;
                    case 'P': 
                        DrawRectangle(x, y, TILE_SIZE, TILE_SIZE, GRAY); 
                        break;
                    default:  
                        break; // Espaço vazio (rio) - já desenhado pelo background
                }
            }
        }
    }
}

void UnloadMap(Map *map) {
    // Nada a liberar ainda, mas evita erro de linkagem
}