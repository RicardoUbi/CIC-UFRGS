#include "obstacle.h"
#include "game.h"

Obstacle CreateObstacle(void)
{
    Obstacle o = {0};
    o.active = true;
    o.pos = (Vector2){GetRandomValue(80, SCREEN_WIDTH - 80), GetRandomValue(-600, -60)};
    o.fuel = (GetRandomValue(0, 10) > 7);
    return o;
}

void UpdateObstacles(Obstacle obstacles[], Bullet bullets[], Player *player)
{
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        if (!obstacles[i].active) continue;

        obstacles[i].pos.y += SCROLL_SPEED;

        if (obstacles[i].pos.y > SCREEN_HEIGHT + 40)
            obstacles[i] = CreateObstacle();

        Rectangle obsRect = {obstacles[i].pos.x, obstacles[i].pos.y, 30, 30};

        if (CheckCollisionRecs(obsRect, player->hitbox))
        {
            if (obstacles[i].fuel)
                player->fuel = 100;
            else
                player->lives--;

            obstacles[i] = CreateObstacle();
        }

        for (int j = 0; j < MAX_BULLETS; j++)
        {
            if (bullets[j].active &&
                CheckCollisionRecs(obsRect, (Rectangle){bullets[j].pos.x, bullets[j].pos.y, 4, 10}))
            {
                bullets[j].active = false;
                obstacles[i] = CreateObstacle();
                player->score += 50;
            }
        }
    }
}

void DrawObstacles(Obstacle obstacles[])
{
    for (int i = 0; i < MAX_OBSTACLES; i++)
    {
        if (!obstacles[i].active) continue;

        if (obstacles[i].fuel)
            DrawRectangle(obstacles[i].pos.x, obstacles[i].pos.y, 30, 30, YELLOW);
        else
            DrawRectangle(obstacles[i].pos.x, obstacles[i].pos.y, 30, 30, RED);
    }
}
