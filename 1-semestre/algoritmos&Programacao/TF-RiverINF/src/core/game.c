#include "game.h"
#include <stdlib.h>
#include <time.h>

Game InitGame(void)
{
    srand(time(NULL));
    Game game = {0};

    game.player = CreatePlayer();
    game.player.pos = (Vector2){SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT - 80};
    game.player.fuel = 100;
    game.player.lives = 3;
    game.player.score = 0;

    game.bgOffset = 0;
    game.gameOver = false;

    for (int i = 0; i < MAX_BULLETS; i++)
        game.bullets[i].active = false;

    for (int i = 0; i < MAX_OBSTACLES; i++)
        game.obstacles[i] = CreateObstacle();

    return game;
}

void UpdateGame(Game *game)
{
    if (game->gameOver)
    {
        if (IsKeyPressed(KEY_R))
            *game = InitGame();
        return;
    }

    UpdatePlayer(&game->player);
    UpdateBullets(game->bullets, &game->player);
    UpdateObstacles(game->obstacles, game->bullets, &game->player);

    game->bgOffset += SCROLL_SPEED;
    if (game->bgOffset >= SCREEN_HEIGHT)
        game->bgOffset = 0;

    if (game->player.lives <= 0)
        game->gameOver = true;
}

void DrawGame(Game *game)
{
    BeginDrawing();
    ClearBackground(DARKBLUE);

    DrawBackground(game->bgOffset);
    DrawObstacles(game->obstacles);
    DrawPlayer(&game->player);
    DrawBullets(game->bullets);
    DrawHUD(&game->player);

    if (game->gameOver)
    {
        DrawText("GAME OVER", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 20, 40, RED);
        DrawText("Press R to Replay", SCREEN_WIDTH / 2 - 110, SCREEN_HEIGHT / 2 + 40, 20, WHITE);
    }

    EndDrawing();
}

void UnloadGame(Game *game)
{
    UnloadPlayer(&game->player);
}
