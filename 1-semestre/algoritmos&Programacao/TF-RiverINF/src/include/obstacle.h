#ifndef OBSTACLE_H
#define OBSTACLE_H
#include "raylib.h"
#include "bullet.h"
#include "player.h"

#define MAX_OBSTACLES 6

typedef struct {
    Vector2 pos;
    bool active;
    bool fuel;
} Obstacle;

Obstacle CreateObstacle(void);
void UpdateObstacles(Obstacle obstacles[], Bullet bullets[], Player *player);
void DrawObstacles(Obstacle obstacles[]);

#endif
