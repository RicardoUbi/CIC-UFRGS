#include "player.h"

#define PLAYER_SPEED 5

Player CreatePlayer(void)
{
    Player p = {0};
    p.pos = (Vector2){240, 640};
    p.fuel = 100;
    p.lives = 3;
    p.score = 0;
    p.scale = 0.5f;

    p.texture = LoadTexture("src/assets/airplanes/air1.png");

    p.hitbox.width = p.texture.width * p.scale * 0.6f;
    p.hitbox.height = p.texture.height * p.scale * 0.7f;
    p.hitbox.x = p.pos.x - p.hitbox.width / 2;
    p.hitbox.y = p.pos.y - p.hitbox.height / 2;

    return p;
}

void UpdatePlayerHitbox(Player *p)
{
    p->hitbox.x = p->pos.x - p->hitbox.width / 2;
    p->hitbox.y = p->pos.y - p->hitbox.height / 2;
}

void UpdatePlayer(Player *p)
{
    if (IsKeyDown(KEY_LEFT) && p->pos.x > 80)
        p->pos.x -= PLAYER_SPEED;
    if (IsKeyDown(KEY_RIGHT) && p->pos.x < 480 - 80)
        p->pos.x += PLAYER_SPEED;

    UpdatePlayerHitbox(p);

    p->fuel -= 0.1f;
    if (p->fuel <= 0)
    {
        p->lives--;
        p->fuel = 100;
    }
}

void DrawPlayer(Player *p)
{
    DrawTextureEx(
        p->texture,
        (Vector2){p->pos.x - p->texture.width * p->scale / 2, p->pos.y - p->texture.height * p->scale / 2},
        0.0f,
        p->scale,
        WHITE
    );

    // Debug: hitbox
    // DrawRectangleLines(p->hitbox.x, p->hitbox.y, p->hitbox.width, p->hitbox.height, RED);
}

void DrawHUD(Player *p)
{
    DrawRectangle(0, 0, 480, 40, (Color){0, 0, 0, 150});
    DrawText(TextFormat("Fuel: %.0f", p->fuel), 20, 10, 20, RAYWHITE);
    DrawText(TextFormat("Lives: %d", p->lives), 180, 10, 20, RAYWHITE);
    DrawText(TextFormat("Score: %d", p->score), 320, 10, 20, RAYWHITE);
}

void UnloadPlayer(Player *p)
{
    UnloadTexture(p->texture);
}
