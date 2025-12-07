#include "raylib.h"
#include <stdio.h>

#include "../include/player.h"
#include "../include/bullet.h"
#include "../include/obstacle.h"
#include "../include/map.h"

static int gameOver = 0;
static float gameOverTimer = 0;
static float blinkTimer = 0;

void DrawTestHUD(const Player *player, const BulletSystem *bulletSystem, const ObstacleSystem *obstacleSystem, const Map *map)
{
    // Fundo do HUD
    DrawRectangle(0, 0, SCREEN_WIDTH, 70, (Color){0, 0, 0, 200});

    // Coluna 1: Player info
    DrawText(TextFormat("SCORE: %06d", player->score), 20, 10, 20, WHITE);
    if (gameOver)
    {
        DrawText("GAME OVER", 20, 35, 24, RED);
    }
    else
    {
        DrawText(TextFormat("LIVES: %d", player->lives), 20, 35, 18,
                 (player->lives > 1) ? GREEN : RED);
    }
    DrawText(TextFormat("SKIN: %d", player->currentSkin + 1), 20, 55, 16, YELLOW);

    // Coluna 2: Combustível
    if (!gameOver) // Só mostra combustível se não for game over
    {
        DrawText("FUEL:", 200, 10, 20, WHITE);
        DrawRectangle(260, 10, 200, 20, RED);
        DrawRectangle(260, 10, (int)(player->fuel * 2), 20, GREEN);
        DrawText(TextFormat("%.0f", player->fuel), 470, 10, 20, WHITE);
    }

    // Coluna 3: Sistema de tiros
    DrawText("BULLETS:", 500, 10, 20, WHITE);
    DrawText(TextFormat("%d/%d", GetActiveBulletCount(bulletSystem), MAX_BULLETS),
             500, 35, 18, (GetActiveBulletCount(bulletSystem) < MAX_BULLETS) ? GREEN : YELLOW);

    // Coluna 4: Obstáculos
    DrawText("OBSTACLES:", 500, 55, 16, WHITE);
    DrawText(TextFormat("%d", GetActiveObstacleCount(obstacleSystem)),
             590, 55, 16, BLUE);

    // Coluna 5: Mapa
    if (!gameOver)
    {
        DrawText(TextFormat("SCROLL: %.1f", map->scrollY), 700, 10, 18, WHITE);
        DrawText(TextFormat("SPEED: %.1f", map->scrollSpeed), 700, 35, 18,
                 (map->scrollSpeed < 0) ? GREEN : RED);
    }
    // Estado do player
    const char *status = player->active ? "ALIVE" : "DEAD";
    Color statusColor = player->active ? GREEN : RED;
    DrawText(status, 850, 10, 20, statusColor);

    // Invincibility
    if (player->invincible)
    {
        DrawText("INVINCIBLE", 850, 35, 16, YELLOW);
        float timeLeft = player->invincibleTimer;
        if (timeLeft > 0)
        {
            DrawText(TextFormat("%.1fs", timeLeft), 850, 55, 14, YELLOW);
        }
    }
}

void DrawGameOverScreen(const Player *player)
{
    // Fundo semi-transparente
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){0, 0, 0, 180});

    // Título GAME OVER
    DrawText("GAME OVER",
             SCREEN_WIDTH / 2 - MeasureText("GAME OVER", 80) / 2,
             SCREEN_HEIGHT / 2 - 100,
             80, RED);

    // Pontuação final
    DrawText(TextFormat("FINAL SCORE: %06d", player->score),
             SCREEN_WIDTH / 2 - MeasureText(TextFormat("FINAL SCORE: %06d", player->score), 40) / 2,
             SCREEN_HEIGHT / 2,
             40, YELLOW);

    // Instruções
    DrawText("Press R to Restart or ESC to Exit",
             SCREEN_WIDTH / 2 - MeasureText("Press R to Restart or ESC to Exit", 20) / 2,
             SCREEN_HEIGHT - 100,
             20, GREEN);
}

// MODIFIQUE a função DrawPlayer para incluir efeito de piscar vermelho:
void DrawPlayerWithEffects(const Player *player)
{
    if (!player->active && gameOver)
    {
        // Efeito de piscar vermelho durante game over
        blinkTimer += GetFrameTime();

        // Pisca a cada 0.3 segundos
        if ((int)(blinkTimer * 3) % 2 == 0)
        {
            // Calcula dimensões escaladas
            float width = player->texture.width * player->scale;
            float height = player->texture.height * player->scale;

            // Posição para centralizar
            float drawX = player->x - width / 2;
            float drawY = player->y - height / 2;

            // Desenha o jogador com tom vermelho
            DrawTextureEx(player->texture,
                          (Vector2){drawX, drawY},
                          0.0f,
                          player->scale,
                          (Color){255, 100, 100, 200}); // Vermelho translúcido

            // Borda vermelha
            DrawRectangleLinesEx((Rectangle){drawX, drawY, width, height},
                                 3, RED);
        }
    }
    else
    {
        // Desenho normal do jogador
        DrawPlayer(player);
    }
}

void DrawDebugControls(void)
{
    int yStart = SCREEN_HEIGHT - 140;
    DrawRectangle(10, yStart, 400, 130, (Color){0, 0, 0, 180});

    DrawText("🎯 CONTROLES DE TESTE:", 20, yStart + 10, 18, YELLOW);
    DrawText("← → : Mover jogador", 20, yStart + 35, 16, WHITE);
    DrawText("ESPAÇO : Atirar", 20, yStart + 55, 16, WHITE);
    DrawText("F      : Reabastecer", 20, yStart + 75, 16, WHITE);
    DrawText("C      : Limpar tiros", 20, yStart + 95, 16, WHITE);
    DrawText("R      : Resetar tudo", 220, yStart + 35, 16, WHITE);
    DrawText("L      : Recarregar obstáculos", 220, yStart + 55, 16, WHITE);
    DrawText("H      : Mostrar hitboxes", 220, yStart + 75, 16, WHITE);
    DrawText("[ ]    : Mudar skin", 220, yStart + 95, 16, WHITE);
    DrawText("I      : Toggle invencibilidade", 220, yStart + 115, 16, WHITE);
}

void DrawObstacleLegend(const ObstacleSystem *system)
{
    int x = SCREEN_WIDTH - 150;
    int y = 80;

    DrawRectangle(x - 10, y - 10, 140, 140, (Color){0, 0, 0, 150});
    DrawText("LEGENDA:", x, y, 16, YELLOW);

    y += 25;

    // Navio
    DrawRectangle(x, y, 15, 15, DARKBLUE);
    DrawText("N - Navio (100 pts)", x + 20, y, 14, WHITE);
    y += 20;

    // Helicóptero
    DrawRectangle(x, y, 15, 15, ORANGE);
    DrawText("X - Helicóptero (200 pts)", x + 20, y, 14, WHITE);
    y += 20;

    // Posto
    DrawRectangle(x, y, 15, 15, RED);
    DrawText("G - Posto (reabastece)", x + 20, y, 14, WHITE);
    y += 20;

    // Ponte
    DrawRectangle(x, y, 15, 15, BROWN);
    DrawText("P - Ponte (700 pts)", x + 20, y, 14, WHITE);
    y += 20;

    // Info adicional
    DrawText(TextFormat("Ativos: %d/%d", GetActiveObstacleCount(system), MAX_ENEMIES),
             x, y, 14, GRAY);
}

void DebugPrintPositions(const Player *player, const ObstacleSystem *system,
                         const BulletSystem *bulletSystem, float scrollY)
{
    static int frameCount = 0;
    frameCount++;

    // A cada 60 frames (1 segundo), mostra informações
    if (frameCount % 60 == 0)
    {
        printf("\n=== DEBUG Frame %d ===\n", frameCount);
        printf("Player: (%.0f, %.0f)\n", player->x, player->y);
        printf("ScrollY: %.0f\n", scrollY);
        printf("Tiros ativos: %d\n", GetActiveBulletCount(bulletSystem));
        printf("Obstáculos ativos: %d\n", GetActiveObstacleCount(system));

        // Mostra posição de alguns obstáculos
        int count = 0;
        for (int i = 0; i < MAX_ENEMIES && count < 3; i++)
        {
            if (system->entities[i].active)
            {
                // Calcula manualmente coordenadas de tela
                float screenX = system->entities[i].x;
                float screenY = system->entities[i].y - scrollY;

                // Obtém nome do tipo
                const char *typeName = "Unknown";
                switch (system->entities[i].type)
                {
                case 'N':
                    typeName = "Navio";
                    break;
                case 'X':
                    typeName = "Helicoptero";
                    break;
                case 'G':
                    typeName = "Gasolina";
                    break;
                case 'P':
                    typeName = "Ponte";
                    break;
                case 'T':
                    typeName = "Terra";
                    break;
                }

                printf("Obstáculo %d [%s]: mundo(%.0f,%.0f) tela(%.0f,%.0f)\n",
                       i, typeName,
                       system->entities[i].x, system->entities[i].y,
                       screenX, screenY);
                count++;
            }
        }
        printf("=====================\n");
    }
}

int main(void)
{
    // Inicialização
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Teste: Sistema de Obstáculos");
    if (!IsWindowReady())
    {
        printf("ERRO: Falha ao criar a janela!\n");
        return 1;
    }
    SetTargetFPS(60);

    // Carrega mapa
    Map mapa;
    LoadMap(&mapa, "src/assets/maps/fase1.txt");
    if (mapa.height == 0)
    {
        LoadMap(&mapa, "fase1.txt");
    }
    mapa.scrollSpeed = 2.0f;

    // Inicializa jogador
    Player jogador;
    InitPlayer(&jogador);

    // Inicializa sistema de tiros
    BulletSystem bulletSystem;
    InitBulletSystem(&bulletSystem);

    // Inicializa sistema de obstáculos - CORRIGIDO
    ObstacleSystem obstacleSystem;
    InitObstacles(&obstacleSystem);

    // Carrega obstáculos do mapa
    LoadObstaclesFromMap(&obstacleSystem, &mapa);

    printf("================================\n");
    printf("🎮 TESTE DO SISTEMA DE OBSTÁCULOS\n");
    printf("================================\n");
    printf("Obstáculos carregados: %d\n", GetActiveObstacleCount(&obstacleSystem));
    printf("\nLegenda:\n");
    printf("  N - Navio (azul)       = 100 pontos\n");
    printf("  X - Helicóptero (laranja) = 200 pontos\n");
    printf("  G - Posto (vermelho)   = Reabastece\n");
    printf("  P - Ponte (marrom)     = 700 pontos\n");
    printf("\nControles:\n");
    printf("  ← →   : Mover jogador\n");
    printf("  ESPAÇO: Atirar\n");
    printf("  F     : Reabastecer\n");
    printf("  C     : Limpar todos os tiros\n");
    printf("  R     : Resetar tudo\n");
    printf("  L     : Recarregar obstáculos do mapa\n");
    printf("  H     : Mostrar/esconder hitboxes\n");
    printf("  I     : Toggle invencibilidade\n");
    printf("  [ ]   : Mudar skin do jogador\n");
    printf("================================\n");

    // Variáveis de teste
    int showHitboxes = 0;
    int godMode = 0;
    gameOver = 0;
    gameOverTimer = 0;
    blinkTimer = 0;

    while (!WindowShouldClose())
    {
        float deltaTime = GetFrameTime();

        if (!gameOver)
        {
            // ===== ATUALIZAÇÕES =====
            UpdateMapScroll(&mapa);
            UpdatePlayer(&jogador);
            UpdateBullets(&bulletSystem);
            UpdateObstacles(&obstacleSystem, mapa.scrollSpeed);

            // ===== COLISÕES =====
            // Tiros vs Obstáculos
            int pointsEarned = CheckBulletObstacleCollision(&obstacleSystem, &bulletSystem, mapa.scrollY);
            if (pointsEarned > 0)
            {
                jogador.score += pointsEarned;
                printf("🎯 +%d pontos! Total: %d\n", pointsEarned, jogador.score);
            }

            // Player vs Obstáculos
            if (!godMode)
            {
                HandlePlayerObstacleCollision(&obstacleSystem, &jogador, mapa.scrollY);
            }

            if (!jogador.active && !godMode)
            {
                gameOver = 1;
                gameOverTimer = 0;
                printf("GAME OVER! Pontuação final: %d\n", jogador.score);
            }

            // Debug
            DebugPrintPositions(&jogador, &obstacleSystem, &bulletSystem, mapa.scrollY);

            // ===== CONTROLES =====
            if (IsKeyPressed(KEY_SPACE) || IsKeyDown(KEY_SPACE))
            {
                FireBulletFromPlayer(&bulletSystem, &jogador);
            }
            if (IsKeyPressed(KEY_F))
                Refuel(&jogador, 30.0f);
            if (IsKeyPressed(KEY_C))
                ClearBullets(&bulletSystem);
            if (IsKeyPressed(KEY_R))
            {
                InitPlayer(&jogador);
                ClearBullets(&bulletSystem);
                LoadObstaclesFromMap(&obstacleSystem, &mapa);
                printf("Tudo resetado!\n");
            }
            if (IsKeyPressed(KEY_L))
            {
                LoadObstaclesFromMap(&obstacleSystem, &mapa);
                printf("🔄 Obstáculos recarregados do mapa\n");
            }
            if (IsKeyPressed(KEY_H))
                showHitboxes = !showHitboxes;
            if (IsKeyPressed(KEY_I))
            {
                godMode = !godMode;
                jogador.invincible = godMode;
                jogador.invincibleTimer = godMode ? 9999.0f : 0;
                printf("%s invencibilidade\n", godMode ? "✅ Ativou" : "⏸️ Desativou");
            }
            if (IsKeyPressed(KEY_RIGHT_BRACKET))
                NextPlayerSkin(&jogador);
            if (IsKeyPressed(KEY_LEFT_BRACKET))
                PreviousPlayerSkin(&jogador);

            // Controle manual da invencibilidade
            if (godMode && jogador.invincibleTimer < 9000)
            {
                jogador.invincibleTimer = 9999.0f;
            }

            // Pontos por sobrevivência
            static float survivalTimer = 0;
            survivalTimer += deltaTime;
            if (survivalTimer >= 1.0f && jogador.active)
            {
                jogador.score += 5;
                survivalTimer = 0;
            }
        }

        if (IsKeyPressed(KEY_R))
        {
            InitPlayer(&jogador);
            ClearBullets(&bulletSystem);
            LoadObstaclesFromMap(&obstacleSystem, &mapa);
            mapa.scrollY = 0;
            gameOver = 0;
            gameOverTimer = 0;
            blinkTimer = 0;
            printf("Tudo resetado!\n");
        }

        // ===== RENDERIZAÇÃO =====
        BeginDrawing();
        {
            ClearBackground(BLUE);

            // Mapa
            int startY = (int)(mapa.scrollY / TILE_SIZE);
            int endY = startY + (SCREEN_HEIGHT / TILE_SIZE) + 1;
            if (endY > mapa.height)
                endY = mapa.height;

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

            // Obstáculos
            DrawObstacles(&obstacleSystem, mapa.scrollY);

            // Tiros
            DrawBullets(&bulletSystem);

            // Jogador
            DrawPlayerWithEffects(&jogador);

            // UI
            DrawTestHUD(&jogador, &bulletSystem, &obstacleSystem, &mapa);
            DrawObstacleLegend(&obstacleSystem);
            DrawDebugControls();

            // Hitboxes
            if (showHitboxes)
            {
                // Jogador
                Rectangle playerHitbox = GetPlayerHitbox(&jogador);
                DrawRectangleLinesEx(playerHitbox, 2, RED);

                // Obstáculos
                for (int i = 0; i < MAX_ENEMIES; i++)
                {
                    if (obstacleSystem.entities[i].active)
                    {
                        Entity *ent = &obstacleSystem.entities[i];
                        float screenY = ent->y - mapa.scrollY;

                        Color hitboxColor = YELLOW;
                        int width = TILE_SIZE;
                        int height = TILE_SIZE;

                        switch (ent->type)
                        {
                        case 'G':
                            hitboxColor = GREEN;
                            width = 4 * TILE_SIZE;
                            break;
                        case 'N':
                            hitboxColor = BLUE;
                            break;
                        case 'X':
                            hitboxColor = ORANGE;
                            break;
                        case 'P':
                            hitboxColor = BROWN;
                            width = 4 * TILE_SIZE;
                            break;
                        }

                        DrawRectangleLines(ent->x, screenY, width, height, hitboxColor);
                    }
                }

                // Tiros
                for (int i = 0; i < MAX_BULLETS; i++)
                {
                    if (bulletSystem.bullets[i].active)
                    {
                        DrawRectangleLines(
                            bulletSystem.bullets[i].x,
                            bulletSystem.bullets[i].y,
                            bulletSystem.bullets[i].width,
                            bulletSystem.bullets[i].height,
                            GRAY);
                    }
                }

                DrawText("HITBOXES: ON", SCREEN_WIDTH - 120, 60, 16, GREEN);
            }

            // Modo God
            if (godMode)
            {
                DrawText("GOD MODE: ON", SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT / 2 - 100, 30, YELLOW);
            }

            if (gameOver)
            {
                DrawGameOverScreen(&jogador);
            }

            DrawText("TESTE DE SISTEMA DE OBSTÁCULOS - Pressione ESC para sair",
                     SCREEN_WIDTH / 2 - 220, SCREEN_HEIGHT - 20, 18, YELLOW);
        }
        EndDrawing();
    }

    // Limpeza
    UnloadBulletSystem(&bulletSystem);
    UnloadPlayerTextures(&jogador);
    CloseWindow();

    printf("================================\n");
    printf("👋 Teste finalizado\n");
    printf("Pontuação final: %d\n", jogador.score);
    printf("Obstáculos restantes: %d\n", GetActiveObstacleCount(&obstacleSystem));
    printf("================================\n");

    return 0;
}