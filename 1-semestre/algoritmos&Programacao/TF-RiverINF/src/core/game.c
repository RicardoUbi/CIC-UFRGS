#include "game.h"
#include "map.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <string.h> // para memcpy se necessário

/*
PurgeObstaclesBeforeWorldY: Game *game, float cutoffY --> void
Objetivo: Remover obstáculos cuja posição mundial worldPos.y é menor
que a posição de corte cutoffY, mantendo a ordem relativa dos obstáculos
restantes.
*/
static void PurgeObstaclesBeforeWorldY(Game *game, float cutoffY)
{
    int write = 0;
    // Percorre todos os obstáculos existentes
    for (int read = 0; read < game->obstacleCount; read++)
    {
        Obstacle *o = &game->obstacles[read];
        if (!o->active) // Pula inativos
            continue;

        // Se a posição Y do obstáculo for maior ou igual ao ponto de corte
        if (o->worldPos.y >= cutoffY)
        {
            // Se o índice de leitura for diferente do de escrita, copia o obstáculo para o novo slot (compactação)
            if (write != read)
                game->obstacles[write] = game->obstacles[read];
            write++; // Avança o índice de escrita
        }
        else
        {
            // O obstáculo foi removido/descartado (worldPos.y < cutoffY)
            if (game->debugMode)
            {
                printf("[PURGE] removendo obstáculo em worldY %.1f (< %.1f)\n", o->worldPos.y, cutoffY);
            }
        }
    }
    game->obstacleCount = write; // Atualiza a contagem total de obstáculos
}

// Cria obstáculos para um nível específico.
// Se append==false: zera lista e cria só do nível indicado.
// Se append==true: adiciona ao final (até MAX_OBSTACLES).
static void CreateObstaclesFromMapLevel(Game *game, int levelIndex, bool append)
{
    if (!append)
    {
        game->obstacleCount = 0;
    }

    printf("Criando obstáculos para nível %d (append=%s). Obsts antes: %d\n",
           levelIndex + 1, append ? "true" : "false", game->obstacleCount);

    Map *map = &game->levels[levelIndex];

    for (int row = 0; row < MAP_ROWS; row++)
    {
        for (int col = 0; col < MAP_COLS; col++)
        {
            char cell = map->data[row][col];

            // Se a sua notação de mapa tiver mais caracteres "colidíveis",
            // inclua aqui. Mantive seus tipos originais e adicionei '#'
            // como exemplo de borda/parede comum.
            if (cell == 'N' || cell == 'X' || cell == 'P' || cell == 'G' || cell == '#')
            {
                if (game->obstacleCount < MAX_OBSTACLES)
                {
                    Obstacle newObstacle = CreateObstacleFromMap(cell, col, row);

                    newObstacle.worldPos.x = col * TILE_SIZE;
                    // **correção importante**: aplicar offset do nível (levelIndex)
                    newObstacle.worldPos.y = row * TILE_SIZE + (levelIndex * WORLD_HEIGHT);
                    newObstacle.active = true;

                    game->obstacles[game->obstacleCount] = newObstacle;
                    game->obstacleCount++;

                    if (game->debugMode)
                    {
                        printf(" -> criado obst %c em (col=%d,row=%d) -> world(%.1f, %.1f)\n",
                               cell, col, row,
                               newObstacle.worldPos.x, newObstacle.worldPos.y);
                    }
                }
                else
                {
                    printf("AVISO: limite de obstáculos atingido ao tentar adicionar [%d,%d]\n", col, row);
                }
            }
        }
    }

    printf("Total de obstáculos após criação: %d\n", game->obstacleCount);
}

// --- Colisões / debug (corrigido para usar world-space consistente) ----

static void HandleCollisions(Game *game)
{
    // Convertendo hitbox do player (screen-space) para WORLD space:
    Rectangle playerWorldRect = game->player.hitbox;
    // screenY -> worldY: worldY = screenY + cameraY   (pois screenY = worldY - cameraY)
    playerWorldRect.y += game->cameraY;

    // PLAYER x OBSTACLE
    for (int i = 0; i < game->obstacleCount; i++)
    {
        Obstacle *obs = &game->obstacles[i];
        if (!obs->active)
            continue;

        Rectangle obsWorldRect = (Rectangle){obs->worldPos.x, obs->worldPos.y, TILE_SIZE, TILE_SIZE};

        if (CheckCollisionRecs(playerWorldRect, obsWorldRect))
        {
            game->player.lives--;
            obs->active = false;
            printf("[COLISÃO] Player x Obstáculo em world(%.1f,%.1f). Vidas agora: %d\n",
                   obs->worldPos.x, obs->worldPos.y, game->player.lives);

            if (game->player.lives <= 0)
            {
                game->gameOver = true;
                printf("=== GAME OVER (por colisão) ===\n");
            }
        }
    }

    // BULLET x OBSTACLE: garantir que bullet pos também esteja em WORLD space
    for (int b = 0; b < MAX_BULLETS; b++)
    {
        Bullet *bullet = &game->bullets[b];
        if (!bullet->active)
            continue;

        // Converter: assumindo que bullet->pos está em SCREEN space (se sua implementação
        // já usa world-space, remova a soma com cameraY). Esta conversão torna o código
        // robusto mesmo se bullets estiverem em screen-space.
        Vector2 bulletWorldPos = {bullet->pos.x, bullet->pos.y + game->cameraY};

        for (int i = 0; i < game->obstacleCount; i++)
        {
            Obstacle *obs = &game->obstacles[i];
            if (!obs->active)
                continue;

            Rectangle obsWorldRect = (Rectangle){obs->worldPos.x, obs->worldPos.y, TILE_SIZE, TILE_SIZE};
            if (CheckCollisionPointRec(bulletWorldPos, obsWorldRect))
            {
                bullet->active = false;
                obs->active = false;
                game->player.score += 10;
                printf("[BALA] Colidiu com obstáculo em (%.1f,%.1f). Score: %d\n",
                       obs->worldPos.x, obs->worldPos.y, game->player.score);
                break;
            }
        }
    }
}

// -----------------------------------------------------------------------

Game InitGame(void)
{
    srand((unsigned)time(NULL));
    Game game = {0};

    game.gameRender = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

    game.cameraY = 0;
    game.currentLevel = 0; // 0-based
    game.levelCompleted = false;
    game.obstacleCount = 0;
    game.debugMode = true;
    game.levelTransitionTimer = 0;
    game.nextLevelLoaded = false;

    // Carregar mapas
    for (int i = 0; i < TOTAL_LEVELS; i++)
    {
        char filename[64];
        sprintf(filename, "src/assets/maps/fase%d.txt", i + 1);
        game.levels[i] = LoadMap(filename);
        printf("Mapa %d carregado: %s\n", i + 1, filename);
    }

    game.player = CreatePlayer();
    game.player.pos = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 80.0f};
    UpdatePlayerHitbox(&game.player);

    for (int i = 0; i < MAX_BULLETS; i++)
        game.bullets[i].active = false;

    // Cria obstáculos apenas do nível inicial (sem append)
    CreateObstaclesFromMapLevel(&game, game.currentLevel, false);

    game.bgOffset = 0;
    game.gameOver = false;

    printf("=== JOGO INICIADO ===\n");
    printf("Level (index): %d\n", game.currentLevel);
    printf("CameraY inicial: %.1f\n", game.cameraY);
    printf("World Height: %d, Screen Height: %d\n", WORLD_HEIGHT, SCREEN_HEIGHT);
    printf("Player position: (%.1f, %.1f)\n", game.player.pos.x, game.player.pos.y);
    printf("Obstáculos criados: %d\n", game.obstacleCount);
    printf("Player: %d vidas, %.0f combustível\n", game.player.lives, game.player.fuel);

    return game;
}

void UpdateGame(Game *game)
{
    // Toggle debug mode
    if (IsKeyPressed(KEY_F1))
    {
        game->debugMode = !game->debugMode;
        printf("Debug mode: %s\n", game->debugMode ? "ON" : "OFF");
    }

    if (game->gameOver)
    {
        if (IsKeyPressed(KEY_R))
        {
            printf("Reiniciando jogo...\n");
            *game = InitGame();
        }
        return;
    }

    UpdatePlayer(&game->player);
    UpdateBullets(game->bullets, &game->player);

    // Move câmera (world scrolling)
    game->cameraY -= SCROLL_SPEED;

    float currentWorldPosition = -game->cameraY;

    float purgeCutoff = game->cameraY - TILE_SIZE * 2;
    PurgeObstaclesBeforeWorldY(game, purgeCutoff);

    float currentLevelStart = game->currentLevel * (float)WORLD_HEIGHT;
    float currentLevelEnd = (game->currentLevel + 1) * (float)WORLD_HEIGHT;
    float currentLevelHeight = (float)WORLD_HEIGHT;

    // Debug do progresso (imprime em intervalos)
    if (game->debugMode)
    {
        int cw_int = (int)currentWorldPosition;
        if (cw_int % 60 == 0)
        {
            float levelProgress = (currentWorldPosition - currentLevelStart) / currentLevelHeight * 100.0f;
            if (levelProgress < 0)
                levelProgress = 0;
            if (levelProgress > 100)
                levelProgress = 100;

            printf("Level %d Progresso: %.1f%% | WorldY: %.1f\n",
                   game->currentLevel + 1, levelProgress, currentWorldPosition);
        }
    }

    // Pré-carregar próximo nível (quando atingir 80% do nível atual)
    if (!game->nextLevelLoaded &&
        game->currentLevel < TOTAL_LEVELS - 1 &&
        currentWorldPosition > currentLevelStart + (WORLD_HEIGHT * 0.8f))
    {
        int nextLevelIndex = game->currentLevel + 1;
        printf("Pré-carregando nível %d (index %d)...\n", nextLevelIndex + 1, nextLevelIndex);
        game->nextLevelLoaded = true;

        // Criar obstáculos do próximo nível (append = true)
        CreateObstaclesFromMapLevel(game, nextLevelIndex, true);

        printf("Nível %d pré-carregado. Total de obstáculos: %d\n", nextLevelIndex + 1, game->obstacleCount);
    }

    // Transição para próximo nível quando passar do nível atual
    if (currentWorldPosition > currentLevelEnd && game->currentLevel < TOTAL_LEVELS - 1)
    {
        game->currentLevel++;
        game->nextLevelLoaded = false;

        // Remover obstáculos de níveis anteriores (para evitar acumulação)
        float keepCutoff = game->currentLevel * (float)WORLD_HEIGHT;
        PurgeObstaclesBeforeWorldY(game, keepCutoff);

        printf("=== TRANSIÇÃO PARA LEVEL %d ===\n", game->currentLevel + 1);
        printf("CameraY: %.1f, WorldPosition: %.1f. Obstáculos agora: %d\n", game->cameraY, currentWorldPosition, game->obstacleCount);
    }

    // Game over se atingir o final de todos os níveis
    if (game->currentLevel >= TOTAL_LEVELS - 1 &&
        currentWorldPosition > TOTAL_LEVELS * (float)WORLD_HEIGHT)
    {
        game->gameOver = true;
        printf("=== FIM DE JOGO - TODAS AS FASES COMPLETADAS ===\n");
    }

    // Colisões (usa world-space consistentemente)
    HandleCollisions(game);

    // Mantém compatibilidade com UpdateObstacles
    UpdateObstacles(game->obstacles, game->obstacleCount, game->bullets, &game->player, game, game->cameraY);

    game->bgOffset += SCROLL_SPEED;
    if (game->bgOffset >= SCREEN_HEIGHT)
        game->bgOffset = 0;

    if (game->player.lives <= 0)
    {
        game->gameOver = true;
        printf("=== GAME OVER ===\n");
        printf("Pontuação final: %d\n", game->player.score);
    }

    game->player.fuel -= 0.015f;
    if (game->player.fuel <= 0)
    {
        game->player.lives--;
        game->player.fuel = 100;
        printf("Combustível zerado! Vida perdida. Vidas restantes: %d\n", game->player.lives);
        if (game->player.lives <= 0)
        {
            game->gameOver = true;
        }
    }
}

void DrawGame(Game *game)
{
    BeginTextureMode(game->gameRender);
    ClearBackground(DARKBLUE);

    DrawBackground(game->bgOffset);

    // Desenhar todos os mapas que estão na viewport
    for (int level = 0; level < TOTAL_LEVELS; level++)
    {
        float levelStartY = level * (float)WORLD_HEIGHT;
        float levelEndY = (level + 1) * (float)WORLD_HEIGHT;

        if (levelEndY > -game->cameraY && levelStartY < -game->cameraY + SCREEN_HEIGHT)
        {
            DrawMap(&game->levels[level], levelStartY + game->cameraY);
        }
    }

    // Draw obstacles (DrawObstacles deve usar screenY = worldY - cameraY internamente)
    DrawObstacles(game->obstacles, game->cameraY);

    DrawPlayer(&game->player);
    DrawBullets(game->bullets);
    DrawHUD(&game->player);

    if (game->debugMode)
    {
        for (int i = 0; i < game->obstacleCount; i++)
        {
            if (game->obstacles[i].active && game->obstacles[i].type == OBSTACLE_BRIDGE)
            {
                float screenY = game->obstacles[i].worldPos.y - game->cameraY;
                if (screenY >= -TILE_SIZE && screenY <= SCREEN_HEIGHT + TILE_SIZE)
                {
                    DrawRectangleLines(
                        game->obstacles[i].worldPos.x,
                        screenY,
                        TILE_SIZE,
                        TILE_SIZE,
                        ORANGE);

                    DrawText("PONTE",
                             (int)game->obstacles[i].worldPos.x,
                             (int)(screenY - 20),
                             10,
                             ORANGE);
                }
            }
        }

        char debugText[128];
        float worldPos = -game->cameraY;
        sprintf(debugText, "Level: %d/%d | WorldY: %.1f | Obst: %d", game->currentLevel + 1, TOTAL_LEVELS, worldPos, game->obstacleCount);
        DrawText(debugText, 10, 30, 20, RED);
    }

    EndTextureMode();

    DrawTextureRec(
        game->gameRender.texture,
        (Rectangle){0, 0, (float)game->gameRender.texture.width, -(float)game->gameRender.texture.height},
        (Vector2){0, 0},
        WHITE);
}

void UnloadGame(Game *game)
{
    UnloadPlayer(&game->player);
    for (int i = 0; i < TOTAL_LEVELS; i++)
    {
        UnloadMap(&game->levels[i]);
    }
    UnloadRenderTexture(game->gameRender);
}
