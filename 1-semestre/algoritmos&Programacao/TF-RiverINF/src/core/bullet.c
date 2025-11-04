#include "bullet.h"

// Alterna o modo de tiro (você pode mudar depois para um power-up, etc.)
static bool doubleShot = true;

void UpdateBullets(Bullet bullets[], Player *player)
{
    // Alternar modo de tiro temporariamente (pressione TAB só pra testar)
    if (IsKeyPressed(KEY_TAB))
        doubleShot = !doubleShot;

    if (IsKeyPressed(KEY_SPACE))
    {
        if (doubleShot)
        {
            // --- TIRO DUPLO ---
            for (int i = 0; i < MAX_BULLETS; i += 2)
            {
                if (!bullets[i].active && i + 1 < MAX_BULLETS)
                {
                    float leftX = player->hitbox.x + player->hitbox.width * 0.25f;
                    float rightX = player->hitbox.x + player->hitbox.width * 0.75f;
                    float y = player->hitbox.y; // sai do topo do hitbox

                    bullets[i].pos = (Vector2){leftX, y};
                    bullets[i].active = true;

                    bullets[i + 1].pos = (Vector2){rightX, y};
                    bullets[i + 1].active = true;
                    break;
                }
            }
        }
        else
        {
            // --- TIRO CENTRAL ---
            for (int i = 0; i < MAX_BULLETS; i++)
            {
                if (!bullets[i].active)
                {
                    float centerX = player->hitbox.x + player->hitbox.width / 2 - 2; // centraliza o tiro
                    float y = player->hitbox.y; // topo do hitbox

                    bullets[i].pos = (Vector2){centerX, y};
                    bullets[i].active = true;
                    break;
                }
            }
        }
    }

    // Atualiza movimento dos tiros
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

    // Debug opcional: mostrar o modo atual
    DrawText(doubleShot ? "Double Shot" : "Single Shot", 10, 60, 20, RAYWHITE);
}
