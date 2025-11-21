#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "player.h"
#include "bullet.h"
#include "obstacle.h"
#include "background.h"
#include "map.h"
#include "defines.h"

#define SCREEN_WIDTH BASE_WIDTH
#define SCREEN_HEIGHT BASE_HEIGHT
#define TOTAL_LEVELS 5

typedef struct Game
{
    Player player;
    Bullet bullets[MAX_BULLETS];
    Obstacle obstacles[MAX_OBSTACLES];
    float bgOffset;
    float cameraY;
    bool gameOver;
    int currentLevel;
    bool levelCompleted;
    int obstacleCount;
    bool debugMode;
    float levelTransitionTimer;
    bool nextLevelLoaded;
    RenderTexture2D gameRender;
    Map levels[TOTAL_LEVELS];
} Game;


Game InitGame(void);
void UpdateGame(Game *game);
void DrawGame(Game *game);
void UnloadGame(Game *game);
void LoadNextLevel(Game *game);
void CreateObstaclesFromMap(Game *game);

#endif