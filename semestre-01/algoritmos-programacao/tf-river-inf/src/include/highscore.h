#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include "defines.h"
#include "raylib.h"

// Gerenciamento de Highscores
void LoadHighScores(HighScoreEntry *highScores);
void SaveHighScores(const HighScoreEntry *highScores);
void AddHighScore(HighScoreEntry *highScores, int newScore, const char *name);
int IsHighScore(const HighScoreEntry *highScores, int newScore);
void SortHighScores(HighScoreEntry *highScores);
void ResetHighScores(HighScoreEntry *highScores);

// Utilitários
void DrawHighScores(const HighScoreEntry *highScores, int x, int y, int fontSize, Color color);
int GetHighScorePosition(const HighScoreEntry *highScores, int newScore);

#endif