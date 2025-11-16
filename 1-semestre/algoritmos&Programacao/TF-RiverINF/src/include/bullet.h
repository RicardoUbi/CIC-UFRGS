#ifndef BULLET_H
#define BULLET_H
#include "raylib.h"
#include "player.h"
#include "defines.h"

typedef struct {
    Vector2 pos;
    bool active;
} Bullet;

void UpdateBullets(Bullet bullets[], Player *player);
void DrawBullets(Bullet bullets[]);

#endif