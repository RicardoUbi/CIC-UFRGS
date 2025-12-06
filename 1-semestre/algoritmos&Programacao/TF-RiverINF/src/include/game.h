#ifndef GAME_H
#define GAME_H

#include "defines.h"
#include "player.h"
#include "bullet.h"
#include "obstacle.h"
#include "map.h"
#include "highscore.h"

#define TOTAL_LEVELS 5

typedef struct {
    GameState currentState;                    // Estado atual do jogo
    int score;                                 // Pontuação do jogador
    int gameWon;                               // Se ganhou todas as fases
    int currentLevel;                          // Número da fase atual
    int totalLevels;                           // Número total de fases
    int isCustomLevel;                         // Se é um mapa personalizado
    char customLevelFile[100];                 // Nome do arquivo do mapa personalizado
    Player player;                             // Jogador
    Map map;                                   // Mapa
    HighScoreEntry highScores[MAX_HIGHSCORES]; // Highscores
} Game;

// Funções de jogo
void InitGame(Game *game);
void UpdateGame(Game *game);
void DrawGame(Game *game);
void ChangeState(Game *game, GameState newState);
void ResetGame(Game *game);
void LoadNextLevel(Game *game);
void IsHighScore(Game *game);

#endif