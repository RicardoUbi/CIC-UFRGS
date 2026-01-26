#ifndef GAME_H
#define GAME_H

#include "defines.h"
#include "player.h"
#include "bullet.h"
#include "obstacle.h"
#include "map.h"
#include "highscore.h"

#define TOTAL_LEVELS 5
#define MAX_NAME_LENGTH 11

typedef struct
{
    GameState currentState;           // Estado atual do jogo
    int score;                        // Pontuação do jogador
    int gameWon;                      // Se ganhou todas as fases
    int currentLevel;                 // Número da fase atual
    int totalLevels;                  // Número total de fases
    int isCustomLevel;                // Se é um mapa personalizado
    char customLevelFile[100];        // Nome do arquivo do mapa personalizado
    char playerName[MAX_NAME_LENGTH]; // Nome do jogador para highscore
    int nameInputPos;                 // Posição do cursor na entrada de nome

    // Sistemas do jogo
    Player player;
    BulletSystem bulletSystem;
    ObstacleSystem obstacleSystem;
    Map map;

    // Highscores
    HighScoreEntry highScores[MAX_HIGHSCORES];

    // Controle de tempo
    float gameTime;
    float levelStartTime;

    // Flags de controle
    int showHitboxes;
    int godMode;
    int gameOver;
    float gameOverTimer;
    float blinkTimer;

    // Tamanho da janela (para responsividade)
    int windowWidth;
    int windowHeight;
    float scaleFactor;

    int pauseType;    // 0 = pausa simples | 1 = confirmar sair
    float pauseTimer; // usado na contagem regressiva
    int pauseCount;   // 3, 2, 1
} Game;

void LoadCampaignLevel(Game *game, int level);
void LoadNextLevel(Game *game);
int CheckLevelCompletion(Game *game);

// Inicialização
void InitGame(Game *game);
void ResetGame(Game *game);
void LoadGameResources(Game *game);
void UnloadGameResources(Game *game);

// Controle de estado
void ChangeState(Game *game, GameState newState);
void UpdateGame(Game *game);
void DrawGame(Game *game);

// Controle de níveis
void LoadLevel(Game *game, const char *levelFile);
void LoadNextLevel(Game *game);
void LoadCampaignLevel(Game *game, int level);
void LoadCustomLevel(Game *game, const char *filename);
int CheckLevelCompletion(Game *game);

// Highscore
void AddCurrentScore(Game *game);

// Input handling
void ProcessMenuInput(Game *game);
void ProcessGameplayInput(Game *game);
void ProcessGameOverInput(Game *game);
void ProcessNameInput(Game *game);
void ProcessCustomLevelInput(Game *game);

// Renderização de estados
void DrawMenuState(Game *game);
void DrawGameplayState(Game *game);
void DrawGameOverState(Game *game);
void DrawVictoryState(Game *game);
void DrawCustomLevelState(Game *game);
void DrawNameInputState(Game *game);
void DrawHighScoreState(Game *game);

// Utilitários
void UpdateWindowSize(Game *game, int width, int height);
void DrawScaled(Game *game, void (*drawFunction)(Game *));

#endif