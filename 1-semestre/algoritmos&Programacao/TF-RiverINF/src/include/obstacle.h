#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "raylib.h"
#include "bullet.h"
#include "player.h"

typedef struct Game Game;

#define MAX_OBSTACLES 100  
#define OBSTACLE_SHIP   0
#define OBSTACLE_HELI   1
#define OBSTACLE_BRIDGE 2
#define OBSTACLE_FUEL   3

typedef struct {
    Vector2 worldPos;
    bool active;
    bool fuel;
    int type;
    int points;
} Obstacle;

Obstacle CreateObstacle(void);
Obstacle CreateObstacleFromMap(char type, int col, int row);
void UpdateObstacles(Obstacle obstacles[], int obstacleCount, Bullet bullets[], Player *player, struct Game *game, float cameraY);
void DrawObstacles(Obstacle obstacles[], float cameraY);

#endif