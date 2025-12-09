#include "raylib.h"
#include "../include/player.h"
#include "../include/map.h"
#include <stdio.h>

void DrawSkinSelection(const Player *player, int showSelector)
{
    if (!showSelector) return;
    
    // Fundo semi-transparente
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 180});
    
    // Título
    DrawText("🎨 SELECIONE SUA SKIN", SCREEN_WIDTH/2 - 150, 50, 30, YELLOW);
    
    // Desenha todas as skins disponíveis
    int skinCount = GetPlayerSkinCount();
    int spacing = 120;
    int startX = SCREEN_WIDTH/2 - ((skinCount * spacing) / 2) + spacing/2;
    
    for (int i = 0; i < skinCount; i++)
    {
        int x = startX + (i * spacing);
        int y = 150;
        
        // Moldura da skin
        Color frameColor = (i == player->currentSkin) ? GREEN : WHITE;
        DrawRectangleLines(x - 45, y - 45, 90, 90, frameColor);
        
        // Número da skin
        DrawText(TextFormat("%d", i + 1), x - 5, y + 60, 20, WHITE);
        
        // Preview da skin (simplificado - cores)
        Color skinColor = 
            (i == 0) ? YELLOW :
            (i == 1) ? RED :
            (i == 2) ? BLUE :
            (i == 3) ? GREEN : PURPLE;
        
        DrawRectangle(x - 30, y - 30, 60, 60, skinColor);
        DrawText("✈️", x - 15, y - 10, 30, BLACK);
    }
    
    // Instruções
    DrawText("← → : Navegar skins", SCREEN_WIDTH/2 - 100, 250, 20, WHITE);
    DrawText("ENTER : Selecionar", SCREEN_WIDTH/2 - 100, 280, 20, WHITE);
    DrawText("ESC   : Sair do seletor", SCREEN_WIDTH/2 - 100, 310, 20, WHITE);
    
    // Skin atual
    DrawText(TextFormat("SKIN ATUAL: %d", player->currentSkin + 1), 
             SCREEN_WIDTH/2 - 80, 350, 25, GREEN);
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Teste: Skins do Player");
    SetTargetFPS(60);
    
    // Carrega mapa
    Map mapa;
    LoadMap(&mapa, "src/assets/maps/fase2.txt");
    mapa.scrollSpeed = 2.0f;
    
    // Inicializa jogador
    Player jogador;
    InitPlayer(&jogador);
    
    // Estado do teste
    int showSkinSelector = 1;  // Começa mostrando seletor
    int skinChanged = 0;
    
    printf("================================\n");
    printf("TESTE DE SKINS DO PLAYER\n");
    printf("================================\n");
    printf("Controles no jogo:\n");
    printf("  ← →  : Mover avião\n");
    printf("  F    : Reabastecer\n");
    printf("  P    : Ativar/Desativar\n");
    printf("  R    : Resetar\n");
    printf("  [ ]  : Mudar skin\n");
    printf("  M    : Mostrar/esconder seletor\n");
    printf("================================\n");
    
    while (!WindowShouldClose())
    {
        // Atualiza mapa
        UpdateMapScroll(&mapa);
        
        // Se estiver no seletor de skins
        if (showSkinSelector)
        {
            // Navegação de skins no seletor
            if (IsKeyPressed(KEY_RIGHT))
            {
                NextPlayerSkin(&jogador);
                skinChanged = 1;
            }
            
            if (IsKeyPressed(KEY_LEFT))
            {
                PreviousPlayerSkin(&jogador);
                skinChanged = 1;
            }
            
            // Seleciona skin e sai do seletor
            if (IsKeyPressed(KEY_ENTER))
            {
                showSkinSelector = 0;
                printf("Skin %d selecionada\n", jogador.currentSkin + 1);
            }
            
            // Sai do seletor sem mudar
            if (IsKeyPressed(KEY_ESCAPE))
            {
                showSkinSelector = 0;
                printf("Sair do seletor de skins\n");
            }
        }
        else
        {
            // Jogo normal
            UpdatePlayer(&jogador);
            
            // Controles de teste
            if (IsKeyPressed(KEY_F)) Refuel(&jogador, 30.0f);
            if (IsKeyPressed(KEY_P)) jogador.active = !jogador.active;
            if (IsKeyPressed(KEY_R)) InitPlayer(&jogador);
            if (IsKeyPressed(KEY_M)) showSkinSelector = 1;  // Volta ao seletor
            
            // Muda skin durante o jogo
            if (IsKeyPressed(KEY_RIGHT_BRACKET)) NextPlayerSkin(&jogador);
            if (IsKeyPressed(KEY_LEFT_BRACKET)) PreviousPlayerSkin(&jogador);
            
            // Adiciona pontos
            if (IsKeyPressed(KEY_SPACE)) jogador.score += 100;
        }
        
        // Renderização
        BeginDrawing();
        {
            // Fundo
            ClearBackground(BLUE);
            
            // Mapa
            int startY = (int)(mapa.scrollY / TILE_SIZE);
            int endY = startY + (SCREEN_HEIGHT / TILE_SIZE) + 1;
            if (endY > mapa.height) endY = mapa.height;
            
            for (int y = startY; y < endY; y++)
            {
                for (int x = 0; x < MAP_COLS; x++)
                {
                    if (mapa.data[y][x] == 'T')
                    {
                        float screenY = (y * TILE_SIZE) - mapa.scrollY;
                        DrawRectangle(x * TILE_SIZE, screenY, TILE_SIZE, TILE_SIZE, GREEN);
                    }
                }
            }
            
            // Jogador (se não estiver no seletor)
            if (!showSkinSelector)
            {
                DrawPlayer(&jogador);
            }
            
            // Seletor de skins (sobreposto)
            DrawSkinSelection(&jogador, showSkinSelector);
            
            // HUD (se não estiver no seletor)
            if (!showSkinSelector)
            {
                DrawRectangle(0, 0, SCREEN_WIDTH, 50, (Color){0, 0, 0, 200});
                
                // Pontuação
                DrawText(TextFormat("SCORE: %06d", jogador.score), 20, 15, 20, WHITE);
                
                // Combustível
                DrawText("FUEL:", 300, 15, 20, WHITE);
                DrawRectangle(360, 15, 200, 20, RED);
                DrawRectangle(360, 15, (int)(jogador.fuel * 2), 20, GREEN);
                DrawText(TextFormat("%.0f", jogador.fuel), 570, 15, 20, WHITE);
                
                // Skin atual
                DrawText(TextFormat("SKIN: %d/%d", 
                        jogador.currentSkin + 1, GetPlayerSkinCount()), 
                        650, 15, 20, YELLOW);
                
                // Vidas
                DrawText(TextFormat("VIDAS: %d", jogador.lives), 800, 15, 20, 
                        (jogador.lives > 1) ? GREEN : RED);
                
                // Instruções
                if (!showSkinSelector)
                {
                    DrawText("[ ]: Mudar skin | M: Menu skins | F: Gasolina | R: Reset", 
                             10, SCREEN_HEIGHT - 25, 18, YELLOW);
                }
            }
            
            // Título do modo atual
            if (showSkinSelector)
            {
                DrawText("SELECIONE SUA SKIN", SCREEN_WIDTH/2 - 120, 10, 25, YELLOW);
            }
            else
            {
                DrawText("MODO JOGO - Pressione M para selecionar skin", 
                         SCREEN_WIDTH/2 - 200, 10, 20, GREEN);
            }
        }
        EndDrawing();
    }
    
    // Limpeza
    UnloadPlayerTextures(&jogador);
    CloseWindow();
    
    printf("================================\n");
    printf("Teste finalizado\n");
    printf("Skin final: %d\n", jogador.currentSkin + 1);
    printf("Pontuação: %d\n", jogador.score);
    printf("================================\n");
    
    return 0;
}