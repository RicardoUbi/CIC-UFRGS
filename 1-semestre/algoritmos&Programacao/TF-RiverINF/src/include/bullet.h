#ifndef BULLET_H
#define BULLET_H

#include "defines.h"

typedef struct {
    Bullet bullets[MAX_BULLETS]; // Array de balas
    int activeBulletsCount;      // Contador de balas ativas
} BulletSystem;

void InitBullets(BulletSystem *bs);
void FireBullet(BulletSystem *bs, float startX, float startY);
void UpdateBullets(BulletSystem *bs);
void DrawBullets(const BulletSystem *bs);
void ClearBullets(BulletSystem *bs);
int CheckBulletCollision(BulletSystem *bs, float objX, float objY, float objWidth, float objHeight);

#endif