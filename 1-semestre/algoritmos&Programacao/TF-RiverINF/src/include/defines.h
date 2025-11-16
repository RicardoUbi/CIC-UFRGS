#ifndef DEFINES_H
#define DEFINES_H

// Tamanho base do jogo (resolução lógica)
#define BASE_WIDTH 768   // Aumentado de 480
#define BASE_HEIGHT 640  // Aumentado de 640

// Tamanho dos tiles (volta para 20 -> compatível com seus arquivos de mapa 24x20)
#define TILE_SIZE 32

// Mapas .txt têm 24x20 caracteres (colunas x linhas)
#define MAP_COLS 24
#define MAP_ROWS 20

// Velocidades
#define SCROLL_SPEED 2.0f
#define PLAYER_SPEED 5
#define BULLET_SPEED 12
#define MAX_BULLETS 20 

#define WORLD_HEIGHT (MAP_ROWS * TILE_SIZE)

#endif
