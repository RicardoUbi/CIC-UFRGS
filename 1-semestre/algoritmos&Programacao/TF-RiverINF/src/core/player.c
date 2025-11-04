#include "player.h"

#define JATO_COR_PRINCIPAL BLUE
#define JATO_COR_DETALHE GRAY
#define JATO_COR_CABINE SKYBLUE
#define JATO_COR_MOTOR DARKGRAY
#define JATO_COR_ASA LIGHTGRAY

Player CreatePlayer(void)
{
    Player p = {0};
    p.pos = (Vector2){240, 640};
    p.fuel = 100;
    p.lives = 3;
    p.score = 0;
    p.scale = 0.5f; // escala do avião

    // --- Carrega a textura do avião ---
    p.texture = LoadTexture("assets/airplanes/air3.png");

    // --- Define o hitbox menor e centralizado ---
    p.hitbox.width = p.texture.width * p.scale * 0.8f;   // 90% da largura da textura
    p.hitbox.height = p.texture.height * p.scale * 1.0f; // 100% da altura
    p.hitbox.x = p.pos.x - p.hitbox.width / 2;
    p.hitbox.y = p.pos.y - p.hitbox.height / 2;

    return p;
}

void DrawPlayer(Player *p)
{
    DrawTextureEx(
        p->texture,
        (Vector2){p->pos.x - p->texture.width * p->scale / 2,
                  p->pos.y - p->texture.height * p->scale / 2},
        0.0f,
        p->scale,
        WHITE);

    // --- opcional: desenhar hitbox para debug ---
    DrawRectangleLines(p->hitbox.x, p->hitbox.y, p->hitbox.width, p->hitbox.height, RED);
}

void UnloadPlayer(Player *p)
{
    UnloadTexture(p->texture);
}

void UpdatePlayerHitbox(Player *p)
{
    p->hitbox.x = p->pos.x - p->hitbox.width / 2;
    p->hitbox.y = p->pos.y - p->hitbox.height / 2;
}
