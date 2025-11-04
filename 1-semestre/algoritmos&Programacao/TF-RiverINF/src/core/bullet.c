#include "bullet.h"

void UpdateBullets(Bullet bullets[], Player *player)
{
    if (IsKeyPressed(KEY_SPACE))
    {
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (!bullets[i].active)
            {
                bullets[i].pos = (Vector2){player->pos.x + 8, player->pos.y - 10};
                bullets[i].active = true;
                break;
            }
        }
    }

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullets[i].active)
        {
            bullets[i].pos.y -= BULLET_SPEED;
            if (bullets[i].pos.y < 0)
                bullets[i].active = false;
        }
    }
}

void DrawBullets(Bullet bullets[])
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullets[i].active)
            DrawRectangle(bullets[i].pos.x, bullets[i].pos.y, 4, 10, ORANGE);
    }
}
