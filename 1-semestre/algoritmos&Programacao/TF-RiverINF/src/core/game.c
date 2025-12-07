#include "game.h"
#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void InitGame(Game *game);
void UpdateGame(Game *game);
void DrawGame(Game *game);
void ChangeState(Game *game, GameState newState);
void ResetGame(Game *game);
void LoadNextLevel(Game *game);
void IsHighScore(Game *game);