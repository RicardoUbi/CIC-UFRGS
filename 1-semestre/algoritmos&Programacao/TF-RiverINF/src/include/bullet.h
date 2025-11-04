#ifndef BULLET_H
#define BULLET_H
#include "raylib.h"
#include "player.h"

#define MAX_BULLETS 10
#define BULLET_SPEED 10

typedef struct {
    Vector2 pos;
    bool active;
} Bullet;

void UpdateBullets(Bullet bullets[], Player *player);
void DrawBullets(Bullet bullets[]);

#endif
