#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "defines.h"
#include "player.h"
#include "bullet.h"
#include "map.h"

typedef struct {
    Entity entities[MAX_ENEMIES];   // Array de entidades
    int activeEntitiesCount;        // Contador de entidades ativas
} ObstacleSystem;

typedef enum {
    OBSTACLE_TYPE_NONE,             // Nenhum
    OBSTACLE_TYPE_TERRA,            // Terra
    OBSTACLE_TYPE_NAVIO,            // Navio
    OBSTACLE_TYPE_HELICOPTERO,      // Helicoptero
    OBSTACLE_TYPE_GAS,              // Gasolina
    OBSTACLE_TYPE_PONTE             // Ponte
} ObstacleType;

// Gerenciamento básico
void InitObstacles(ObstacleSystem *os);
void AddObstacle(ObstacleSystem *os, float x, float y, ObstacleType type);
void DrawObstacles(const ObstacleSystem *os, float scrollY);
void ClearObstacles(ObstacleSystem *os);
Entity *GetObstacleAt(ObstacleSystem *os, float x, float y);

// Carregamento
void LoadObstaclesFromMap(ObstacleSystem* system, const Map* map);

// Colisões
int CheckBulletObstacleCollision(ObstacleSystem* obsSystem, BulletSystem* bulletSystem, float scrollY);
void HandlePlayerObstacleCollision(ObstacleSystem* system, Player* player, float scrollY);

// Consultas
int GetActiveObstacleCount(const ObstacleSystem* system);
ObstacleType GetObstacleType(const Entity* entity);
const char* GetObstacleTypeName(ObstacleType type);
int GetObstacleScoreValue(ObstacleType type);

#endif