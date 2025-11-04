#ifndef PLAYER_H
#define PLAYER_H

#include "raylib.h"

typedef struct Player {
    Vector2 pos;
    Texture2D texture;
    float scale;
    Rectangle hitbox;
    float fuel;
    int lives;
    int score;
} Player;


Player CreatePlayer(void);
void DrawPlayer(Player *p);
void UnloadPlayer(Player *p);
void UpdatePlayerHitbox(Player *p);

#endif
