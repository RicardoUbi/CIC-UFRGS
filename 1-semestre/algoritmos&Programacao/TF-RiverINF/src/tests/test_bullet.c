// src/tests/test_bullet.c
#include "raylib.h"
#include <stddef.h>
#include <stdio.h>
#include "../include/player.h"
#include "../include/bullet.h"
#include "../include/map.h"

void DrawTestHUD(const Player* player, const BulletSystem* bulletSystem, const Map* map)
{
    // Fundo do HUD
    DrawRectangle(0, 0, SCREEN_WIDTH, 60, (Color){0, 0, 0, 200});
    
    // Coluna 1: Player info
    DrawText(TextFormat("SCORE: %06d", player->score), 20, 10, 20, WHITE);
    DrawText(TextFormat("SKIN: %d", player->currentSkin + 1), 20, 35, 18, YELLOW);
    
    // Coluna 2: Combustível
    DrawText("FUEL:", 200, 10, 20, WHITE);
    DrawRectangle(260, 10, 200, 20, RED);
    DrawRectangle(260, 10, (int)(player->fuel * 2), 20, GREEN);
    DrawText(TextFormat("%.0f", player->fuel), 470, 10, 20, WHITE);
    
    // Coluna 3: Sistema de tiros
    DrawText("BULLETS:", 500, 10, 20, WHITE);
    DrawText(TextFormat("ACTIVE: %d/%d", 
            GetActiveBulletCount(bulletSystem), MAX_BULLETS), 
            500, 35, 18, 
            (GetActiveBulletCount(bulletSystem) < MAX_BULLETS * 0.8) ? GREEN : RED);
    
    // Coluna 4: Mapa
    DrawText(TextFormat("SCROLL: %.1f", map->scrollY), 700, 10, 18, WHITE);
    DrawText(TextFormat("SPEED: %.1f", map->scrollSpeed), 700, 35, 18, 
            (map->scrollSpeed < 0) ? GREEN : RED);
    
    // Estado do player
    const char* status = player->active ? "ALIVE" : "DEAD";
    Color statusColor = player->active ? GREEN : RED;
    DrawText(status, 850, 10, 20, statusColor);
    DrawText(TextFormat("LIVES: %d", player->lives), 850, 35, 18, 
            (player->lives > 1) ? GREEN : ORANGE);
}

void DrawDebugControls(void)
{
    int yStart = SCREEN_HEIGHT - 120;
    DrawRectangle(10, yStart, 350, 110, (Color){0, 0, 0, 180});
    
    DrawText("🎯 CONTROLES DE TESTE:", 20, yStart + 10, 18, YELLOW);
    DrawText("← → : Mover jogador", 20, yStart + 35, 16, WHITE);
    DrawText("ESPAÇO : Atirar", 20, yStart + 55, 16, WHITE);
    DrawText("F      : Reabastecer (+30)", 20, yStart + 75, 16, WHITE);
    DrawText("C      : Limpar tiros", 20, yStart + 95, 16, WHITE);
    DrawText("R      : Resetar tudo", 180, yStart + 35, 16, WHITE);
    DrawText("P      : Pausar jogador", 180, yStart + 55, 16, WHITE);
    DrawText("[ ]    : Mudar skin", 180, yStart + 75, 16, WHITE);
}

void DrawTargetsForTesting(BulletSystem* bulletSystem)
{
    // Cria alguns alvos estáticos para teste de colisão
    static Rectangle targets[5] = {
        {200, 100, 40, 40},
        {400, 150, 40, 40},
        {600, 200, 40, 40},
        {300, 300, 40, 40},
        {500, 250, 40, 40}
    };
    
    static Color targetColors[5] = {RED, BLUE, GREEN, PURPLE, ORANGE};
    static int targetHealth[5] = {3, 3, 3, 3, 3};
    
    // Desenha alvos
    for (int i = 0; i < 5; i++)
    {
        if (targetHealth[i] > 0)
        {
            DrawRectangleRec(targets[i], targetColors[i]);
            DrawText(TextFormat("%d", targetHealth[i]),
                     targets[i].x + 10, targets[i].y + 10, 20, WHITE);
            
            // Verifica colisão com tiros
            Bullet* hitBullet = GetCollidingBullet(bulletSystem,
                targets[i].x, targets[i].y, targets[i].width, targets[i].height);
            
            if (hitBullet != NULL)
            {
                targetHealth[i]--;
                hitBullet->active = 0;  // Destroi o tiro
                
                printf("Alvo %d atingido! Vida: %d\n", i + 1, targetHealth[i]);
                
                if (targetHealth[i] <= 0)
                {
                    printf("Alvo %d destruído!\n", i + 1);
                }
            }
        }
    }
}

int main(void)
{
    // Inicialização
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Teste: Sistema de Tiros");
    SetTargetFPS(60);
    
    // Inicializa áudio (para sons dos tiros)
    InitAudioDevice();
    
    // Carrega mapa
    Map mapa;
    LoadMap(&mapa, "src/assets/maps/fase2.txt");
    mapa.scrollSpeed = 2.0f;
    
    // Inicializa jogador
    Player jogador;
    InitPlayer(&jogador);
    
    // Inicializa sistema de tiros
    BulletSystem bulletSystem;
    InitBulletSystem(&bulletSystem);
    
    printf("================================\n");
    printf("TESTE DO SISTEMA DE TIROS\n");
    printf("================================\n");
    printf("Controles:\n");
    printf("  ← →   : Mover jogador\n");
    printf("  ESPAÇO: Atirar\n");
    printf("  F     : Reabastecer\n");
    printf("  C     : Limpar todos os tiros\n");
    printf("  R     : Resetar tudo\n");
    printf("  [ ]   : Mudar skin do jogador\n");
    printf("================================\n");
    printf("Alvos coloridos: atire neles!\n");
    printf("Cada alvo leva 3 tiros para ser destruído\n");
    printf("================================\n");
    
    // Variáveis de teste
    int showHitboxes = 0;
    int autoFire = 0;
    float autoFireTimer = 0;
    
    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();
        
        // Atualiza mapa
        UpdateMapScroll(&mapa);
        
        // Atualiza jogador
        UpdatePlayer(&jogador);
        
        // Atualiza tiros
        UpdateBullets(&bulletSystem);
        
        // ===== CONTROLES =====
        
        // Atirar (espaço ou auto-fire)
        if (IsKeyPressed(KEY_SPACE) || (autoFire && autoFireTimer <= 0))
        {
            FireBulletFromPlayer(&bulletSystem, &jogador);
            if (autoFire) autoFireTimer = 0.1f;  // Auto-fire rápido
        }
        
        // Auto-fire toggle
        if (IsKeyPressed(KEY_T))
        {
            autoFire = !autoFire;
            printf("%s auto-fire\n", autoFire ? "Ativou" : "Desativou");
        }
        
        // Atualiza timer do auto-fire
        if (autoFire)
        {
            autoFireTimer -= deltaTime;
            if (autoFireTimer < 0) autoFireTimer = 0;
        }
        
        // Outros controles
        if (IsKeyPressed(KEY_F)) Refuel(&jogador, 30.0f);
        if (IsKeyPressed(KEY_P)) jogador.active = !jogador.active;
        if (IsKeyPressed(KEY_R))
        {
            InitPlayer(&jogador);
            ClearBullets(&bulletSystem);
            printf("Tudo resetado!\n");
        }
        if (IsKeyPressed(KEY_C))
        {
            ClearBullets(&bulletSystem);
            printf("Todos os tiros limpos\n");
        }
        if (IsKeyPressed(KEY_H)) showHitboxes = !showHitboxes;
        if (IsKeyPressed(KEY_RIGHT_BRACKET)) NextPlayerSkin(&jogador);
        if (IsKeyPressed(KEY_LEFT_BRACKET)) PreviousPlayerSkin(&jogador);
        
        // Adiciona pontos automaticamente (para teste)
        static float scoreTimer = 0;
        scoreTimer += deltaTime;
        if (scoreTimer >= 1.0f)  // A cada segundo
        {
            jogador.score += 10;  // Pontos por sobrevivência
            scoreTimer = 0;
        }
        
        // ===== RENDERIZAÇÃO =====
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
            
            // Alvos para teste
            DrawTargetsForTesting(&bulletSystem);
            
            // Tiros (sobre o mapa, sob o jogador)
            DrawBullets(&bulletSystem);
            
            // Jogador (sobre os tiros)
            DrawPlayer(&jogador);
            
            // HUD
            DrawTestHUD(&jogador, &bulletSystem, &mapa);
            
            // Controles de debug
            DrawDebugControls();
            
            // Info adicional
            DrawText(TextFormat("DELTA TIME: %.3f", deltaTime), 
                     SCREEN_WIDTH - 150, SCREEN_HEIGHT - 50, 16, WHITE);
            
            if (showHitboxes)
            {
                DrawText("HITBOXES: ON", SCREEN_WIDTH - 150, SCREEN_HEIGHT - 30, 16, GREEN);
                
                // Mostra hitboxes dos tiros
                for (int i = 0; i < MAX_BULLETS; i++)
                {
                    if (bulletSystem.bullets[i].active)
                    {
                        DrawRectangleLines(
                            bulletSystem.bullets[i].x,
                            bulletSystem.bullets[i].y,
                            bulletSystem.bullets[i].width,
                            bulletSystem.bullets[i].height,
                            GREEN);
                    }
                }
            }
            
            // Auto-fire status
            if (autoFire)
            {
                DrawText("AUTO-FIRE: ON", SCREEN_WIDTH - 150, 100, 20, YELLOW);
            }
            
            // Instrução final
            DrawText("TESTE DE SISTEMA DE TIROS - Pressione ESC para sair",
                     SCREEN_WIDTH/2 - 200, SCREEN_HEIGHT - 20, 18, YELLOW);
        }
        EndDrawing();
    }
    
    // Limpeza
    UnloadBulletSystem(&bulletSystem);
    UnloadPlayerTextures(&jogador);
    CloseAudioDevice();
    CloseWindow();
    
    printf("================================\n");
    printf("Teste finalizado\n");
    printf("Tiros ativos no final: %d\n", GetActiveBulletCount(&bulletSystem));
    printf("Pontuação final: %d\n", jogador.score);
    printf("================================\n");
    
    return 0;
}