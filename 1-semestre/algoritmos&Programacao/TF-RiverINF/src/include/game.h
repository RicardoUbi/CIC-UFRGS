#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "player.h"
#include "bullet.h"
#include "obstacle.h"
#include "background.h"
#include "map.h"

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 640
#define SCROLL_SPEED 3

typedef struct Game {
    Player player;
    Bullet bullets[MAX_BULLETS];
    Obstacle obstacles[MAX_OBSTACLES];
    Map map;
    float bgOffset;
    bool gameOver;
} Game;

Game InitGame(void);
void UpdateGame(Game *game);
void DrawGame(Game *game);
void UnloadGame(Game *game);

#endif
