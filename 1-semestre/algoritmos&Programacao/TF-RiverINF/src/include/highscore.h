#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include "defines.h"

void LoadHighScores(HighScoreEntry *highScores);
void SaveHighScores(const HighScoreEntry *highScores);
void AddHighScore(HighScoreEntry *highScores, int newScore, const char *name);
int IsHighScore(int newScore);
void SortHighScores(HighScoreEntry *highScores);
void ResetHighScores(HighScoreEntry *highScores);

#endif