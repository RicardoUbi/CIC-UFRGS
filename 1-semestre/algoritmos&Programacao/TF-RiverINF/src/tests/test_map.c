#include <stdio.h>
#include "raylib.h"
#include "../include/map.h"

void DrawDebugGrid(void)
{
    // Grade vertical (colunas)
    for (int x = 0; x <= SCREEN_WIDTH; x += TILE_SIZE)
    {
        DrawLine(x, 0, x, SCREEN_HEIGHT, 
                (x % (TILE_SIZE * 5) == 0) ? RED : DARKGRAY);
        
        // Números das colunas
        if (x % (TILE_SIZE * 5) == 0 && x < SCREEN_WIDTH - 40)
        {
            DrawText(TextFormat("%d", x/TILE_SIZE), x + 5, 5, 15, WHITE);
        }
    }
    
    // Grade horizontal (linhas)
    for (int y = 0; y <= SCREEN_HEIGHT; y += TILE_SIZE)
    {
        DrawLine(0, y, SCREEN_WIDTH, y,
                (y % (TILE_SIZE * 5) == 0) ? RED : DARKGRAY);
    }
    
    // Centro da tela
    DrawLine(SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT, YELLOW);
    DrawLine(0, SCREEN_HEIGHT/2, SCREEN_WIDTH, SCREEN_HEIGHT/2, YELLOW);
    
    // Borda da tela
    DrawRectangleLines(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
}

int main(void)
{
    // Inicializa janela
    InitWindow(960, 800, "Teste Mapa - River Raid");
    SetTargetFPS(60);
    
    // Carrega mapa
    Map mapa;
    LoadMap(&mapa, "src/assets/maps/fase2.txt");
    
    // Se não encontrar na pasta assets, tenta na raiz
    if (mapa.height == 0) {
        //printf("Tentando carregar da pasta raiz...\n");
        LoadMap(&mapa, "fase1.txt");
    }
    
    //printf("Mapa carregado: %d linhas\n", mapa.height);
    //printf("Primeira linha: ");
    for (int i = 0; i < 24; i++) {
        //printf("%c", mapa.data[0][i]);
    }
    //printf("\n");
    
    // Loop principal
    while (!WindowShouldClose())
    {
        // Atualiza scroll
        UpdateMapScroll(&mapa);
        
        // Renderização
        BeginDrawing();
        {
            ClearBackground(BLUE);  // Fundo azul (rio)
            DrawDebugGrid();
            DrawMap(&mapa);         // Desenha mapa
            
            // Info na tela
            DrawText(TextFormat("Scroll: %.1f", mapa.scrollY), 10, 10, 20, WHITE);
            DrawText(TextFormat("Linhas: %d", mapa.height), 10, 40, 20, WHITE);
            DrawText(TextFormat("Velocidade: %.1f", mapa.scrollSpeed), 10, 70, 20, WHITE);
            DrawText("Teste do Módulo Map - Pressione ESC para sair", 200, 760, 20, YELLOW);
        }
        EndDrawing();
    }
    
    // Encerra
    CloseWindow();
    //printf("Teste finalizado\n");
    return 0;
}