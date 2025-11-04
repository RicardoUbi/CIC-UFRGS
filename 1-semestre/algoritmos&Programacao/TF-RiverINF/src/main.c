#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include "player.h"

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 640

#define PLAYER_SPEED 5
#define BULLET_SPEED 10
#define SCROLL_SPEED 3
#define MAX_BULLETS 10
#define MAX_OBSTACLES 6
#define MAX_FUEL 5

#define JATO_COR_PRINCIPAL BLUE
#define JATO_COR_DETALHE GRAY
#define JATO_COR_CABINE SKYBLUE
#define JATO_COR_MOTOR DARKGRAY
#define JATO_COR_ASA LIGHTGRAY

typedef struct
{
    Vector2 pos;
    bool active;
} Bullet;

typedef struct
{
    Vector2 pos;
    bool active;
    bool fuel;
} Obstacle;

// Função para criar obstáculos (barcos / blocos / combustível)
Obstacle CreateObstacle(void)
{
    Obstacle o = {0};
    o.active = true;
    o.pos = (Vector2){GetRandomValue(80, SCREEN_WIDTH - 80), GetRandomValue(-600, -60)};
    o.fuel = (GetRandomValue(0, 10) > 7); // 30% chance de ser posto de combustível
    return o;
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "RIVER-INF (Protótipo)");
    InitAudioDevice();
    SetTargetFPS(60);
    srand(time(NULL));

    bool gameOver = false;

    // Fundo
    float bgOffset = 0;

    // Player
    Player player = CreatePlayer();
    player.pos = (Vector2){SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT - 80};
    player.fuel = 100;
    player.lives = 3;
    player.score = 0;

    // Tiros
    Bullet bullets[MAX_BULLETS] = {0};

    // Obstáculos
    Obstacle obstacles[MAX_OBSTACLES] = {0};
    for (int i = 0; i < MAX_OBSTACLES; i++)
        obstacles[i] = CreateObstacle();

    while (!WindowShouldClose())
    {

        if (!gameOver)
        {

            // Movimento do jogador
            if (IsKeyDown(KEY_LEFT) && player.pos.x > 80)
                player.pos.x -= PLAYER_SPEED;
            if (IsKeyDown(KEY_RIGHT) && player.pos.x < SCREEN_WIDTH - 80)
                player.pos.x += PLAYER_SPEED;

            UpdatePlayerHitbox(&player);

            // Disparo
            if (IsKeyPressed(KEY_SPACE))
            {
                for (int i = 0; i < MAX_BULLETS; i++)
                {
                    if (!bullets[i].active)
                    {
                        bullets[i].pos = (Vector2){player.pos.x + 8, player.pos.y - 10};
                        bullets[i].active = true;
                        break;
                    }
                }
            }

            // Atualiza tiros
            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (bullets[i].active)
                {
                    bullets[i].pos.y -= BULLET_SPEED;
                    if (bullets[i].pos.y < 0)
                        bullets[i].active = false;
                }
            }

            // Atualiza obstáculos
            for (int i = 0; i < MAX_OBSTACLES; i++)
            {
                if (obstacles[i].active)
                {
                    obstacles[i].pos.y += SCROLL_SPEED;

                    // Saiu da tela → reaparece acima
                    if (obstacles[i].pos.y > SCREEN_HEIGHT + 40)
                    {
                        obstacles[i] = CreateObstacle();
                    }

                    // Colisão com jogador
                    Rectangle obsRect = {obstacles[i].pos.x, obstacles[i].pos.y, 30, 30};
                    Rectangle playerRect = player.hitbox;

                    if (CheckCollisionRecs(obsRect, playerRect))
                    {
                        if (obstacles[i].fuel)
                        {
                            player.fuel = 100;
                        }
                        else
                        {
                            player.lives--;
                        }
                        obstacles[i] = CreateObstacle();
                    }

                    // Colisão com tiros
                    for (int j = 0; j < MAX_BULLETS; j++)
                    {
                        if (bullets[j].active && CheckCollisionRecs(obsRect, (Rectangle){bullets[j].pos.x, bullets[j].pos.y, 4, 10}))
                        {
                            bullets[j].active = false;
                            obstacles[i] = CreateObstacle();
                            player.score += 50;
                        }
                    }
                }
            }

            // Consumo de combustível
            player.fuel -= 0.1f;
            if (player.fuel <= 0)
            {
                player.lives--;
                player.fuel = 100;
            }

            // Atualiza scroll de fundo
            bgOffset += SCROLL_SPEED;
            if (bgOffset >= SCREEN_HEIGHT)
                bgOffset = 0;
        }

        BeginDrawing();
        ClearBackground(DARKBLUE);

        // Áreas de terra
        DrawRectangle(0, 0, 80, SCREEN_HEIGHT, GREEN);
        DrawRectangle(SCREEN_WIDTH - 80, 0, 80, SCREEN_HEIGHT, GREEN);

        // “Fundo do rio” (efeito scroll simples)
        DrawRectangle(80, (int)bgOffset, SCREEN_WIDTH - 160, SCREEN_HEIGHT, (Color){0, 102, 204, 255});
        DrawRectangle(80, (int)bgOffset - SCREEN_HEIGHT, SCREEN_WIDTH - 160, SCREEN_HEIGHT, (Color){0, 102, 204, 255});

        // Obstáculos
        for (int i = 0; i < MAX_OBSTACLES; i++)
        {
            if (obstacles[i].active)
            {
                if (obstacles[i].fuel)
                    DrawRectangle(obstacles[i].pos.x, obstacles[i].pos.y, 30, 30, YELLOW);
                else
                    DrawRectangle(obstacles[i].pos.x, obstacles[i].pos.y, 30, 30, RED);
            }
        }

        DrawPlayer(&player);

        // Tiros
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (bullets[i].active)
                DrawRectangle(bullets[i].pos.x, bullets[i].pos.y, 4, 10, ORANGE);
        }

        // HUD
        DrawRectangle(0, 0, SCREEN_WIDTH, 40, (Color){0, 0, 0, 150});
        DrawText(TextFormat("Fuel: %.0f", player.fuel), 20, 10, 20, RAYWHITE);
        DrawText(TextFormat("Lives: %d", player.lives), 180, 10, 20, RAYWHITE);
        DrawText(TextFormat("Score: %d", player.score), 320, 10, 20, RAYWHITE);

        if (player.lives <= 0)
        {
            gameOver = true;
        }

        if (gameOver)
        {
            DrawText("GAME OVER", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 20, 40, RED);
            DrawText("Press R to Replay", SCREEN_WIDTH / 2 - 110, SCREEN_HEIGHT / 2 + 40, 20, WHITE);

            if (IsKeyPressed(KEY_R))
            {
                // Reinicia tudo
                player = CreatePlayer();
                player.pos = (Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT - 80};
                player.fuel = 100;
                player.lives = 3;
                player.score = 0;

                for (int i = 0; i < MAX_OBSTACLES; i++)
                    obstacles[i] = CreateObstacle();

                for (int i = 0; i < MAX_BULLETS; i++)
                    bullets[i].active = false;

                bgOffset = 0;
                gameOver = false;
            }
        }

        EndDrawing();
    }

    CloseAudioDevice();
    UnloadPlayer(&player);
    CloseWindow();
    return 0;
}
