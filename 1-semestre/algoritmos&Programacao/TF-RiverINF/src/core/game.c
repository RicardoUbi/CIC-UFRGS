#include "game.h"
#include "map.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

Game InitGame(void)
{
    srand(time(NULL));
    Game game = {0};

    game.gameRender = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

    game.cameraY = 0;
    game.currentLevel = 1;
    game.levelCompleted = false;
    game.obstacleCount = 0;
    game.debugMode = true;
    game.levelTransitionTimer = 0;

    char filename[50];
    sprintf(filename, "src/assets/maps/fase%d.txt", game.currentLevel);
    game.map = LoadMap(filename);

    game.player = CreatePlayer();

    CreateObstaclesFromMap(&game);

    for (int i = 0; i < MAX_BULLETS; i++)
        game.bullets[i].active = false;

    game.bgOffset = 0;
    game.gameOver = false;

    printf("=== JOGO INICIADO ===\n");
    printf("Level: %d\n", game.currentLevel);
    printf("Obstáculos criados: %d\n", game.obstacleCount);
    printf("Player: %d vidas, %.0f combustível\n", game.player.lives, game.player.fuel);

    return game;
}

void CreateObstaclesFromMap(Game *game)
{
    game->obstacleCount = 0;

    printf("Analisando mapa para criar obstáculos...\n");

    for (int row = 0; row < MAP_ROWS; row++)
    {
        for (int col = 0; col < MAP_COLS; col++)
        {
            char cell = game->map.data[row][col];

            if (cell == 'N' || cell == 'X' || cell == 'P' || cell == 'G')
            {
                if (game->obstacleCount < MAX_OBSTACLES)
                {
                    Obstacle newObstacle = CreateObstacleFromMap(cell, col, row);

                    newObstacle.worldPos.x = col * TILE_SIZE;
                    newObstacle.worldPos.y = row * TILE_SIZE;
                    newObstacle.active = true;

                    game->obstacles[game->obstacleCount] = newObstacle;
                    game->obstacleCount++;

                    printf("Criado obstáculo: %c em (%d, %d) -> worldPos(%d, %d)\n",
                           cell, col, row,
                           (int)newObstacle.worldPos.x, (int)newObstacle.worldPos.y);
                }
                else
                {
                    printf("AVISO: Limite de obstáculos atingido em [%d,%d]\n", col, row);
                }
            }
        }
    }

    printf("Total de obstáculos criados: %d\n", game->obstacleCount);
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

    if (game->levelCompleted)
    {
        game->cameraY = WORLD_HEIGHT - SCREEN_HEIGHT;
        game->levelTransitionTimer += GetFrameTime();
        if (game->levelTransitionTimer >= 2.0f) // 2 segundos
        {
            game->levelTransitionTimer = 0;
            LoadNextLevel(game);
        }
        return;
    }

    UpdatePlayer(&game->player);
    UpdateBullets(game->bullets, &game->player);

    if (!game->levelCompleted && !game->gameOver)
    {
        game->cameraY -= SCROLL_SPEED;
        
        if (game->cameraY >= WORLD_HEIGHT - SCREEN_HEIGHT)
        {
            game->cameraY = WORLD_HEIGHT - SCREEN_HEIGHT;
        }
    }

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

void LoadNextLevel(Game *game)
{
    game->currentLevel++;
    char filename[50];
    sprintf(filename, "src/assets/maps/fase%d.txt", game->currentLevel);

    printf("Tentando carregar: %s\n", filename);

    FILE *file = fopen(filename, "r");
    if (file)
    {
        fclose(file);
        game->map = LoadMap(filename);
        game->levelCompleted = false;
        game->bgOffset = 0;

        // Reposicionar jogador
        game->cameraY = 0;
        game->player.pos = (Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT - 80};
        UpdatePlayerHitbox(&game->player);

        // Recriar obstáculos
        CreateObstaclesFromMap(game);

        printf("=== LEVEL %d CARREGADO ===\n", game->currentLevel);
        printf("Obstáculos ativos: %d\n", game->obstacleCount);
    }
    else
    {
        printf("Não há mais fases. Vitória!\n");
        // Em vez de game over, podemos mostrar tela de vitória
        game->gameOver = true;
        // Você pode adicionar uma tela de vitória aqui depois
    }
}

void DrawGame(Game *game)
{
    BeginTextureMode(game->gameRender);

    ClearBackground(DARKBLUE);
    DrawBackground(game->bgOffset);
    DrawMap(&game->map, game->cameraY);
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
                    // Destacar ponte com contorno
                    DrawRectangleLines(
                        game->obstacles[i].worldPos.x,
                        screenY,
                        TILE_SIZE,
                        TILE_SIZE,
                        ORANGE);
                    
                    // Texto indicando ponte
                    DrawText("PONTE", 
                             game->obstacles[i].worldPos.x, 
                             screenY - 20, 
                             10, 
                             ORANGE);
                }
            }
        }
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
    UnloadMap(&game->map);
    UnloadRenderTexture(game->gameRender);
}