#include "player.h"
#include "game.h"  // Adicionado para SCREEN_WIDTH
#include "defines.h"  // Adicionado

Player CreatePlayer(void)
{
    Player p = {0};
    p.pos = (Vector2){SCREEN_WIDTH / 2, SCREEN_HEIGHT - 80};  // Centralizado
    p.fuel = 100;
    p.lives = 3;
    p.score = 0;
    p.scale = 0.5f;

    p.texture = LoadTexture("src/assets/airplanes/air3.png");

    p.hitbox.width = p.texture.width * p.scale * 0.8f;
    p.hitbox.height = p.texture.height * p.scale * 0.8f;
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
    int margin = 5 * TILE_SIZE; // 5 tiles de margem (100 pixels se TILE_SIZE=20)
    
    if (IsKeyDown(KEY_LEFT) && p->pos.x > margin + p->hitbox.width/2)
        p->pos.x -= PLAYER_SPEED;
    if (IsKeyDown(KEY_RIGHT) && p->pos.x < SCREEN_WIDTH - margin - p->hitbox.width/2)
        p->pos.x += PLAYER_SPEED;

    UpdatePlayerHitbox(p);

    // Combustível ainda mais lento
    p->fuel -= 0.015f;
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

    // Debug: hitbox (descomente se necessário)
    // DrawRectangleLines(p->hitbox.x, p->hitbox.y, p->hitbox.width, p->hitbox.height, RED);
}

void DrawHUD(Player *p)
{
    DrawRectangle(0, 0, SCREEN_WIDTH, 40, (Color){0, 0, 0, 150});
    DrawText(TextFormat("Fuel: %.0f", p->fuel), 20, 10, 20, RAYWHITE);
    DrawText(TextFormat("Lives: %d", p->lives), 180, 10, 20, RAYWHITE);
    DrawText(TextFormat("Score: %d", p->score), 320, 10, 20, RAYWHITE);
}

void UnloadPlayer(Player *p)
{
    UnloadTexture(p->texture);
}