// Tela
#define SCREEN_WIDTH 960
#define SCREEN_HEIGHT 800
#define TILE_SIZE 40

// Mapa
#define MAP_COLS 24
#define MAP_MAX_ROWS 1000

// Jogador e inimigos
#define MAX_ENEMIES 100
#define MAX_BULLETS 10
#define MAX_HIGHSCORES 10

// Arquivo
#define HIGHSCORE_FILE "highscore.bin"

// Estados de jogo
typedef enum
{
    MENU,           // Menu
    GAMEPLAY,       // Jogo em andamento
    GAME_OVER,      // Derrota
    VICTORY,        // Vitoria
    CUSTOM_LEVEL,   // Fase personalizada
    NAME_INPUT,     // Entrada de nome
    HIGHSCORE,      // Highscores
} GameState;

// Estruturas base
typedef struct
{
    float x, y;     // Posição no mapa
    int active;     // Ativo ou inativo
} Bullet;

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
