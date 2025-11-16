#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include "raylib.h"

#define MAX_HIGHSCORES 10

typedef struct {
    char name[20];
    int score;
} Highscore;

void LoadHighscores(Highscore highscores[]);
void SaveHighscores(Highscore highscores[]);
void AddHighscore(Highscore highscores[], int score, const char *name);
int CheckHighscore(Highscore highscores[], int score);

#endif