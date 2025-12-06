#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "defines.h"

typedef struct {
    Entity entities[MAX_ENEMIES];   // Array de entidades
    int activeEntitiesCount;        // Contador de entidades ativas
} ObstacleSystem;

typedef enum {
    OBSTACLE_TYPE_TERRA,            // Terra
    OBSTACLE_TYPE_NAVIO,            // Navio
    OBSTACLE_TYPE_HELICOPTERO,      // Helicoptero
    OBSTACLE_TYPE_GAS,              // Gasolina
    OBSTACLE_TYPE_PONTE             // Ponte
} ObstacleType;

void InitObstacles(ObstacleSystem *os);
void AddObstacle(ObstacleSystem *os, float x, float y, ObstacleType type);
void UpdateObstacles(ObstacleSystem *os, float scrollSpeed);
void DrawObstacles(const ObstacleSystem *os);
void ClearObstacles(ObstacleSystem *os);
Entity *GetObstacleAt(ObstacleSystem *os, float x, float y);

#endif