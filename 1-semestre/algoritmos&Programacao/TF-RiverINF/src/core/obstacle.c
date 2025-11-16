#include <stdio.h>
#include "obstacle.h"
#include "game.h"  // Agora Game está completamente definido

Obstacle CreateObstacle(void)
{
    Obstacle o = {0};
    o.active = true;
    o.worldPos = (Vector2){GetRandomValue(100, SCREEN_WIDTH - 100), GetRandomValue(-600, -60)};
    o.fuel = (GetRandomValue(0, 10) > 7);
    return o;
}

Obstacle CreateObstacleFromMap(char type, int col, int row)
{
    Obstacle o = {0};
    o.active = true;

    switch (type)
    {
        case 'N': 
            o.type = OBSTACLE_SHIP; 
            o.points = 100;
            break;
        case 'X': 
            o.type = OBSTACLE_HELI; 
            o.points = 200;
            break;
        case 'P': 
            o.type = OBSTACLE_BRIDGE; 
            o.points = 500;
            break;
        case 'G': 
            o.type = OBSTACLE_FUEL; 
            o.points = 0;
            o.fuel = true;
            break;
    }

    o.worldPos.x = col * TILE_SIZE;
    o.worldPos.y = row * TILE_SIZE;

    return o;
}

void UpdateObstacles(Obstacle obstacles[], int obstacleCount, Bullet bullets[], Player *player, Game *game, float cameraY)
{
    bool bridgeDestroyed = false;

    for (int i = 0; i < obstacleCount; i++)
    {
        if (!obstacles[i].active) continue;

        float screenY = obstacles[i].worldPos.y - cameraY;
        
        if (screenY < -TILE_SIZE || screenY > SCREEN_HEIGHT + TILE_SIZE) {
            continue;
        }

        Rectangle obsRect = {
            obstacles[i].worldPos.x,
            screenY,
            TILE_SIZE,
            TILE_SIZE
        };

        if (screenY >= 0 && screenY <= SCREEN_HEIGHT - TILE_SIZE)
        {
        if (CheckCollisionRecs(obsRect, player->hitbox))
            {
                printf("Colisão com obstáculo tipo %d em (%d, %d)\n", 
                       obstacles[i].type, 
                       (int)obstacles[i].worldPos.x / TILE_SIZE, 
                       (int)obstacles[i].worldPos.y / TILE_SIZE);

                if (obstacles[i].type == OBSTACLE_FUEL)
                {
                    player->fuel = 100;
                    printf("Combustível reabastecido!\n");
                    obstacles[i].active = false;
                }
                else if (obstacles[i].type != OBSTACLE_BRIDGE)
                {
                    player->lives--;
                    printf("Vida perdida! Vidas restantes: %d\n", player->lives);
                    obstacles[i].active = false;
                }
            }
        }

        for (int j = 0; j < MAX_BULLETS; j++)
        {
            if (bullets[j].active)
            {
                Rectangle bulletRect = {
                    bullets[j].pos.x,
                    bullets[j].pos.y,
                    4, 10
                };

                if (CheckCollisionRecs(obsRect, bulletRect))
                {
                    printf("Tiro acertou obstáculo tipo %d\n", obstacles[i].type);

                    bullets[j].active = false;

                    if (obstacles[i].type == OBSTACLE_BRIDGE)
                    {
                        bridgeDestroyed = true;
                        player->score += obstacles[i].points;
                        printf("Ponte destruída! +%d pontos\n", obstacles[i].points);
                    }
                    else if (obstacles[i].type != OBSTACLE_FUEL)
                    {
                        player->score += obstacles[i].points;
                        printf("Inimigo destruído! +%d pontos\n", obstacles[i].points);
                    }

                    obstacles[i].active = false;
                }
            }
        }
    }

    // Check if level should advance
    if (bridgeDestroyed)
    {
        game->levelCompleted = true;
        printf("=== LEVEL COMPLETED ===\n");
    }
}

void DrawObstacles(Obstacle obstacles[], float cameraY)
{
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        if (!obstacles[i].active) continue;

        // === CORREÇÃO: CONVERTER COORDENADAS MUNDIAIS PARA TELA ===
        float screenY = obstacles[i].worldPos.y - cameraY;
        
        // Só desenhar se estiver dentro ou perto da tela
        if (screenY < -TILE_SIZE || screenY > SCREEN_HEIGHT + TILE_SIZE) {
            continue;
        }

        Color color;
        switch (obstacles[i].type)
        {
            case OBSTACLE_SHIP: color = RED; break;
            case OBSTACLE_HELI: color = MAROON; break;
            case OBSTACLE_BRIDGE: color = GRAY; break;
            case OBSTACLE_FUEL: color = YELLOW; break;
            default: color = WHITE; break;
        }

        DrawRectangle(obstacles[i].worldPos.x, screenY, TILE_SIZE, TILE_SIZE, color);
    }
}

