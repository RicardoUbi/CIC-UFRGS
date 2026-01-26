#ifndef DEFINES_H      // ← GUARDA DE INCLUSÃO
#define DEFINES_H
// Tela
#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 800
#define TILE_SIZE 40

// Mapa
#define MAP_COLS 24
#define MAP_ROWS 20

// Jogador e inimigos
#define MAX_SKINS 3
#define PLAYER_SCALE 0.3f
#define HITBOX_SCALE 0.8f
#define MAX_ENEMIES (20*24)
#define MAX_BULLETS 10
#define MAX_HIGHSCORES 10

// Arquivo
#define HIGHSCORE_FILE "highscore.bin"

// Estados de jogo
typedef enum
{
    MENU,           // Menu
    GAMEPLAY,       // Jogo em campanha
    GAME_OVER,      // Derrota
    VICTORY,        // Vitoria
    CUSTOM_LEVEL,   // Fase personalizada
    NAME_INPUT,     // Entrada de nome
    HIGHSCORE,      // Highscores
    GAME_PAUSED,    // Jogo pausado
} GameState;

// Estruturas base
typedef struct
{
    float x, y;     // Posição no mapa
    char type;      // Tipo de entidade: 'T' (Terra), 'N' (Navio), 'X' (Helicoptero), 'G' (Gasolina), 'P' (Ponte)
    int active;     // Ativo ou inativo
} Entity;

typedef struct
{
    char name[20];  // Nome
    int score;      // Pontuação
} HighScoreEntry;

#endif