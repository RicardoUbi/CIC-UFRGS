#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// --- CONSTANTES E DEFINIÇÕES ---
#define SCREEN_WIDTH 960  // 24 blocos * 40 pixels
#define SCREEN_HEIGHT 800 // 20 blocos * 40 pixels
#define TILE_SIZE 40
#define MAP_WIDTH 24        // Largura em caracteres
#define MAX_MAP_HEIGHT 1000 // Altura máxima do mapa (linhas no txt)
#define MAX_ENEMIES 100
#define MAX_BULLETS 10
#define MAX_HIGHSCORES 10
#define HIGHSCORE_FILE "highscore.bin"

// --- ESTRUTURAS ---
typedef struct
{
    float x, y;
    int active;
} Bullet;

typedef struct
{
    float x, y;
    char type; // 'N' (Navio), 'X' (Heli), 'G' (Gas), 'P' (Ponte)
    int active;
} Entity;

typedef struct
{
    char name[20];
    int score;
} HighScoreEntry;

typedef enum
{
    MENU,
    GAMEPLAY,
    GAME_OVER,
    VICTORY,
    NAME_INPUT,
    RANKING,
    CUSTOM_LEVEL
} GameState;

// --- VARIÁVEIS GLOBAIS ---
char mapData[MAX_MAP_HEIGHT][MAP_WIDTH];
int mapHeight = 0; // Quantas linhas foram carregadas
float scrollY = 0; // Posição de rolagem vertical

// Jogador
float playerX = SCREEN_WIDTH / 2;
float playerY = SCREEN_HEIGHT - 100;
float fuel = 100.0f;
int score = 0;
int playerActive = 1;

// Listas
Entity enemies[MAX_ENEMIES];
int enemyCount = 0;
Bullet bullets[MAX_BULLETS];

// Estado do Jogo
GameState currentState = MENU;
char playerNameInput[20] = {0};
int letterCount = 0;
HighScoreEntry highScores[MAX_HIGHSCORES];
int currentLevel = 1;
int totalLevels = 3;            // Número de fases
int gameWon = 0;                // Se ganhou todas as fases vai estar true
char customLevelFile[100] = ""; // Nome do arquivo personalizado
int isCustomLevel = 0;          // 0 = modo campanha, 1 = modo personalizado

// --- FUNÇÕES DE ARQUIVO ---

// Carrega o mapa do TXT
void LoadMap(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f)
    {
        printf("Erro ao abrir %s. Criando mapa vazio.\n", filename);
        return;
    }

    char line[100];
    mapHeight = 0;
    enemyCount = 0;

    while (fgets(line, sizeof(line), f) && mapHeight < MAX_MAP_HEIGHT)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            char c = line[x];
            // Se for nova linha ou fim, preenche com vazio ou Terra
            if (c == '\n' || c == '\0')
                c = ' ';

            // Lógica de spawn de objetos
            if (c == 'N' || c == 'X' || c == 'G' || c == 'P')
            {
                if (enemyCount < MAX_ENEMIES)
                {
                    enemies[enemyCount].x = x * TILE_SIZE;
                    // Y é invertido inicialmente, ajustamos no draw
                    enemies[enemyCount].y = (mapHeight)*TILE_SIZE;
                    enemies[enemyCount].type = c;
                    enemies[enemyCount].active = 1;
                    enemyCount++;
                }
                // No grid lógico, mantemos o caractere para colisão de terreno (G e P)
                // N e X viram água no grid para não colidir como parede
                if (c == 'N' || c == 'X')
                    mapData[mapHeight][x] = ' ';
                else
                    mapData[mapHeight][x] = c;
            }
            else
            {
                mapData[mapHeight][x] = c;
            }
        }
        mapHeight++;
    }
    fclose(f);
}

// Carrega e Salva Highscores
void LoadHighScores()
{
    FILE *f = fopen(HIGHSCORE_FILE, "rb");
    if (f)
    {
        fread(highScores, sizeof(HighScoreEntry), MAX_HIGHSCORES, f);
        fclose(f);
    }
    else
    {
        // Inicializa vazio se não existir
        for (int i = 0; i < MAX_HIGHSCORES; i++)
        {
            strcpy(highScores[i].name, "---");
            highScores[i].score = 0;
        }
    }
}

void SaveHighScores()
{
    FILE *f = fopen(HIGHSCORE_FILE, "wb");
    if (f)
    {
        fwrite(highScores, sizeof(HighScoreEntry), MAX_HIGHSCORES, f);
        fclose(f);
    }
}

void AddHighScore(int newScore, const char *name)
{
    // Insere ordenado
    for (int i = 0; i < MAX_HIGHSCORES; i++)
    {
        if (newScore > highScores[i].score)
        {
            // Move os outros para baixo
            for (int j = MAX_HIGHSCORES - 1; j > i; j--)
            {
                highScores[j] = highScores[j - 1];
            }
            highScores[i].score = newScore;
            strcpy(highScores[i].name, name);
            break;
        }
    }
    SaveHighScores();
}

int IsHighScore(int newScore)
{
    return (newScore > highScores[MAX_HIGHSCORES - 1].score);
}

// --- LÓGICA DO JOGO ---
int CountAvailableLevels()
{
    int count = 0;
    char filename[20];

    for (int i = 1; i <= 10; i++)
    { // Assume no máximo 10 fases
        sprintf(filename, "fase%d.txt", i);
        FILE *f = fopen(filename, "r");
        if (f)
        {
            count++;
            fclose(f);
        }
        else
        {
            break;
        }
    }

    return count;
}

// Função para mostrar arquivos .txt disponíveis
void ShowAvailableLevels()
{
// Sistema específico para cada plataforma
// Para Windows:
#ifdef _WIN32
    system("dir *.txt /b > levels.txt");
    FILE *f = fopen("levels.txt", "r");
#elif __linux__
    system("ls *.txt > levels.txt");
    FILE *f = fopen("levels.txt", "r");
#endif

    if (f)
    {
        char line[100];
        int y = 400;
        while (fgets(line, sizeof(line), f))
        {
            // Remove newline
            line[strcspn(line, "\n")] = 0;
            if (strlen(line) > 0)
            {
                DrawText(line, 220, y, 18, BLUE);
                y += 25;
            }
        }
        fclose(f);
        remove("levels.txt");
    }
}

int ValidateMapFile(const char *filename)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        return 0;

    char line[100];
    int lineCount = 0;
    int valid = 1;

    while (fgets(line, sizeof(line), f) && lineCount < 100)
    {
        int len = strlen(line);
        // Remove newline no final
        if (line[len - 1] == '\n')
            len--;

        // Verifica se tem 24 colunas
        if (len != MAP_WIDTH)
        {
            valid = 0;
            break;
        }

        // Verifica bordas (primeiras e últimas 5 colunas devem ser 'T')
        for (int i = 0; i < 5; i++)
        {
            if (line[i] != 'T' || line[MAP_WIDTH - 1 - i] != 'T')
            {
                valid = 0;
                break;
            }
        }

        lineCount++;
    }

    // Verifica se tem pelo menos 20 linhas
    if (lineCount < 20)
        valid = 0;

    fclose(f);
    return valid;
}

// --- SEPARAÇÃO DE FUNÇÕES ---

void ResetGame()
{
    playerX = SCREEN_WIDTH / 2;
    playerY = SCREEN_HEIGHT - 100;
    fuel = 100.0f;
    score = 0;
    playerActive = 1;
    scrollY = 0;
    enemyCount = 0;
    currentLevel = 1;
    gameWon = 0;

    // Limpa balas
    for (int i = 0; i < MAX_BULLETS; i++)
        bullets[i].active = 0;

    LoadMap("fase1.txt"); // Recarrega para resetar inimigos
}

void LoadNextLevel()
{
    currentLevel++;

    if (currentLevel <= totalLevels)
    {
        // Constrói o nome do arquivo da próxima fase
        char filename[20];
        sprintf(filename, "fase%d.txt", currentLevel);

        // Reseta algumas variáveis para a nova fase
        scrollY = 0;
        enemyCount = 0;

        // Limpa balas
        for (int i = 0; i < MAX_BULLETS; i++)
            bullets[i].active = 0;

        // Carrega o novo mapa
        LoadMap(filename);

        // Reposiciona o jogador (opcional: pode manter na posição atual)
        playerX = SCREEN_WIDTH / 2;
        // playerY permanece o mesmo

        printf("Carregando fase %d: %s\n", currentLevel, filename);
    }
    else
    {
        // Venceu todas as fases!
        gameWon = 1;
        playerActive = 0; // Para parar o jogo
        printf("Parabéns! Você venceu todas as fases!\n");
    }
}

void UpdateGame()
{
    static int frame = 0;
    if (frame < 10)
    { // Mostra apenas nos primeiros 10 frames
        printf("Frame %d: Player at (%.0f, %.0f), ScrollY: %.0f, Fuel: %.1f\n",
               frame, playerX, playerY, scrollY, fuel);
        frame++;
    }

    if (!playerActive)
        return;

    // 1. Movimento do Jogador
    if (IsKeyDown(KEY_LEFT))
        playerX -= 5;
    if (IsKeyDown(KEY_RIGHT))
        playerX += 5;

    if (playerX < 0)
        playerX = 0;
    if (playerX > SCREEN_WIDTH - 30)
        playerX = SCREEN_WIDTH - 30;

    // Avanço do mapa (Scroll)
    float scrollSpeed = 2.0f + (currentLevel * 0.5f);
    scrollY += scrollSpeed;

    // Combustível: consome 10 a cada tela (800 pixels)
    // 10 un / 800 px = 0.0125 un/px. Como andamos 2px/frame -> 0.025
    fuel -= 0.025f;
    if (fuel <= 0)
    {
        playerActive = 0; // Game Over por falta de combustível
    }

    // 2. Tiros
    if (IsKeyPressed(KEY_SPACE))
    {
        for (int i = 0; i < MAX_BULLETS; i++)
        {
            if (!bullets[i].active)
            {
                bullets[i].x = playerX + 15; // Centro do avião
                bullets[i].y = playerY;
                bullets[i].active = 1;
                break;
            }
        }
    }

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullets[i].active)
        {
            bullets[i].y -= 10;
            if (bullets[i].y < 0)
                bullets[i].active = 0;
        }
    }

    // 3. Colisões
    Rectangle playerRect = {playerX, playerY, 30, 30}; // Hitbox menor que 40 para ser justo

    // Colisão com Inimigos e Itens
    for (int i = 0; i < enemyCount; i++)
    {
        if (!enemies[i].active)
            continue;

        // Posição na tela do inimigo (Y original - Scroll invertido)
        // O mapa "desce", então Y aumenta visualmente
        float screenY = enemies[i].y + scrollY;

        // Remove se saiu da tela
        if (screenY > SCREEN_HEIGHT)
            enemies[i].active = 0;

        Rectangle enemyRect = {enemies[i].x, screenY - SCREEN_HEIGHT, TILE_SIZE, TILE_SIZE};

        // Ajuste no sistema de coordenadas: O mapa começa lá em cima (negativo) e vem descendo
        // Vamos simplificar: Y do objeto no mundo é fixo. A câmera sobe (scrollY aumenta).
        // Desenho: (ObjY - ScrollY). Mas para River Raid, o avião é fixo e o mundo desce.
        // Vamos usar: Posição Y real do objeto na tela = ObjetoY - (MapHeight*TileSize) + ScrollY + ScreenHeight
        // Para simplificar muito: Vamos apenas calcular a posição relativa na tela.

        float relativeY = enemies[i].y + scrollY - (mapHeight * TILE_SIZE) + SCREEN_HEIGHT;
        // Nota: essa lógica de scroll infinito requer cuidado. Vamos fazer simples:
        // O mapa carrega de cima pra baixo.

        enemyRect.y = relativeY;

        // Colisão Bala -> Inimigo
        for (int b = 0; b < MAX_BULLETS; b++)
        {
            if (bullets[b].active)
            {
                Rectangle bulletRect = {bullets[b].x, bullets[b].y, 5, 10};
                if (CheckCollisionRecs(bulletRect, enemyRect))
                {
                    if (enemies[i].type == 'N' || enemies[i].type == 'X')
                    {
                        enemies[i].active = 0;
                        bullets[b].active = 0;
                        score += (enemies[i].type == 'N') ? 50 : 100;
                    }
                    else if (enemies[i].type == 'P')
                    {
                        enemies[i].active = 0; // Destroi ponte
                        bullets[b].active = 0;
                        score += 200;
                    }
                }
            }
        }

        // Colisão Jogador -> Inimigo/Obstaculo
        if (CheckCollisionRecs(playerRect, enemyRect))
        {
            if (enemies[i].type == 'N' || enemies[i].type == 'X' || enemies[i].type == 'P')
            {
                playerActive = 0; // Bateu
            }
            if (enemies[i].type == 'G')
            {
                fuel += 1.0f; // Reabastece rápido enquanto passa por cima
                if (fuel > 100)
                    fuel = 100;
            }
        }
    }

    // Colisão com Terreno (Tilemap)
    // Verifica os tiles ao redor do jogador
    int mapBaseY = (int)((mapHeight * TILE_SIZE) - scrollY - SCREEN_HEIGHT) / TILE_SIZE;
    // Conversão de coordenadas é complexa em scroll. Vamos checar colisão pixel a pixel nas bordas
    // Simplificação: Checar cantos do player

    // Verifica limites da tela
    if (playerX < 0 || playerX > SCREEN_WIDTH - TILE_SIZE)
        playerActive = 0;

    // Checar tiles estáticos (Terra 'T')
    // Mapear posição do player na tela para índice na matriz mapData
    // Como o mapa está rolando, precisamos saber qual linha do mapa está na posição Y do player
    int gridY = (int)((scrollY + playerY) / TILE_SIZE);
    int gridX = (int)(playerX / TILE_SIZE);

    if (gridY >= 0 && gridY < mapHeight && gridX >= 0 && gridX < MAP_WIDTH)
    {
        if (mapData[gridY][gridX] == 'T')
        {
            playerActive = 0;
            return; // Sai para evitar mais processamento
        }

        // Verifica o lado direito do avião também
        int gridX2 = (int)((playerX + 30) / TILE_SIZE);
        if (gridX2 < MAP_WIDTH && mapData[gridY][gridX2] == 'T')
        {
            playerActive = 0;
            return;
        }
    }

    // Fim da fase
    if (scrollY >= mapHeight * TILE_SIZE)
    {
        if (isCustomLevel)
        {
            gameWon = 1;
            playerActive = 0;
            score += 500;
        }
        else
        {
            if (currentLevel < totalLevels)
            {
                // Salva o estado atual antes de carregar nova fase
                float savedFuel = fuel;
                int savedScore = score;

                // Carrega próxima fase
                LoadNextLevel();

                // Mantém combustível e pontuação entre fases
                fuel = savedFuel;
                score = savedScore;

                // Adiciona bônus por completar fase
                score += 500;                         // Bônus por completar a fase
                fuel = (fuel < 50) ? fuel + 50 : 100; // Recarga parcial se estiver baixo

                printf("Fase %d completa! Carregando fase %d\n", currentLevel - 1, currentLevel);
            }
            else
            {
                // Última fase completa
                gameWon = 1;
                playerActive = 0;
                score += 1000; // Bônus extra por vencer o jogo
            }
        }
    }
}

void DrawGame()
{
    ClearBackground(BLUE);

    // Desenhar Mapa (Apenas o que está visível)
    // Otimização básica: não desenhar as 1000 linhas, só as 20 da tela
    float offset = (mapHeight * TILE_SIZE) - scrollY - SCREEN_HEIGHT;

    for (int y = 0; y < mapHeight; y++)
    {
        float drawY = (y * TILE_SIZE) - offset;

        // Se estiver fora da tela, pula
        if (drawY < -TILE_SIZE || drawY > SCREEN_HEIGHT)
            continue;

        for (int x = 0; x < MAP_WIDTH; x++)
        {
            char type = mapData[y][x];
            if (type == 'T')
            {
                DrawRectangle(x * TILE_SIZE, drawY, TILE_SIZE, TILE_SIZE, GREEN);
            }
        }
    }

    // Desenhar Entidades (Inimigos)
    for (int i = 0; i < enemyCount; i++)
    {
        if (!enemies[i].active)
            continue;
        float drawY = enemies[i].y - offset;

        Color c = WHITE;
        if (enemies[i].type == 'N')
            c = DARKBLUE; // Navio
        if (enemies[i].type == 'X')
            c = ORANGE; // Heli
        if (enemies[i].type == 'G')
            c = RED; // Gasolina
        if (enemies[i].type == 'P')
            c = BLACK; // Ponte

        DrawRectangle(enemies[i].x, drawY, TILE_SIZE, TILE_SIZE, c);

        // Letra para identificar (Sprite placeholder)
        char s[2] = {enemies[i].type, '\0'};
        DrawText(s, enemies[i].x + 10, drawY + 10, 20, WHITE);
    }

    // Desenhar Player
    DrawRectangle(playerX, playerY, 30, 30, YELLOW); // Avião
    DrawText("A", playerX + 10, playerY + 5, 20, BLACK);

    // Desenhar Balas
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (bullets[i].active)
        {
            DrawRectangle(bullets[i].x, bullets[i].y, 4, 10, WHITE);
        }
    }

    // UI (HUD)
    DrawRectangle(0, 0, SCREEN_WIDTH, 40, DARKGRAY);
    DrawText(TextFormat("SCORE: %06d", score), 20, 10, 20, WHITE);
    if (isCustomLevel)
    {
        // Extrai apenas o nome do arquivo (sem caminho)
        char fileName[50];
        strcpy(fileName, customLevelFile);

        // Remove caminho se houver
        char *lastSlash = strrchr(fileName, '/');
        if (!lastSlash)
            lastSlash = strrchr(fileName, '\\');
        if (lastSlash)
        {
            strcpy(fileName, lastSlash + 1);
        }

        DrawText(TextFormat("FASE: %s", fileName), 600, 10, 20, YELLOW);
    }
    else
    {
        DrawText(TextFormat("FASE: %d/%d", currentLevel, totalLevels), 600, 10, 20, WHITE);
    }

    // Barra de Combustível
    DrawText("FUEL:", 300, 10, 20, WHITE);
    DrawRectangle(360, 10, 200, 20, RED);
    DrawRectangle(360, 10, (int)(fuel * 2), 20, GREEN);

    // Mostra posição atual do jogador no mapa
    int debugRow = (int)((scrollY + playerY) / TILE_SIZE);
    int debugCol = (int)(playerX / TILE_SIZE);
    DrawText(TextFormat("Map Pos: [%d,%d]", debugCol, debugRow), 600, 10, 20, WHITE);
}

// Função auxiliar para desenhar estados na textura alvo
void RenderToTarget(RenderTexture2D target)
{
    BeginTextureMode(target);
    // Desenha o estado atual aqui
    switch (currentState)
    {
    case MENU:
        ClearBackground(BLACK);
        DrawText("RIVER RAID C", 300, 150, 40, GREEN);
        DrawText("1. Pressione ENTER para Campanha", 280, 300, 20, WHITE);
        DrawText("2. Pressione C para Fase Personalizada", 280, 350, 20, WHITE);
        DrawText("3. Pressione R para Ranking", 280, 400, 20, WHITE);
        DrawText("4. Pressione ESC para Sair", 280, 450, 20, WHITE);
        break;

    case GAME_OVER:
        ClearBackground(BLACK);
        DrawText("GAME OVER", 350, 300, 40, RED);
        DrawText(TextFormat("Score Final: %d", score), 360, 360, 20, WHITE);

        if (IsHighScore(score))
        {
            DrawText("NOVO RECORDE! Pressione ENTER", 300, 450, 20, YELLOW);
        }
        else
        {
            DrawText("Pressione ENTER para Menu", 320, 450, 20, WHITE);
        }
        break;

    case VICTORY:
        ClearBackground(BLACK);
        DrawText("VITÓRIA!", 400, 250, 50, GREEN);
        DrawText("PARABÉNS! VOCÊ COMPLETOU TODAS AS FASES!", 200, 320, 25, WHITE);
        DrawText(TextFormat("Score Final: %d", score), 360, 380, 30, YELLOW);

        if (IsHighScore(score))
        {
            DrawText("NOVO RECORDE! Pressione ENTER", 300, 450, 20, YELLOW);
        }
        else
        {
            DrawText("Pressione ENTER para Menu", 320, 500, 20, WHITE);
        }
        DrawText(TextFormat("Fases completas: %d/%d", currentLevel - 1, totalLevels), 320, 420, 20, LIGHTGRAY);
        break;

    case CUSTOM_LEVEL:
        ClearBackground(BLACK);
        DrawText("MODO FASE PERSONALIZADA", 280, 100, 30, YELLOW);
        DrawText("Digite o nome do arquivo (ex: minha_fase.txt):", 200, 200, 20, WHITE);

        // Caixa de texto
        DrawRectangle(200, 250, 500, 40, DARKGRAY);
        DrawText(customLevelFile, 210, 260, 25, WHITE);

        // Cursor piscante
        if (((int)(GetTime() * 2) % 2) == 0)
        {
            int textWidth = MeasureText(customLevelFile, 25);
            DrawRectangle(210 + textWidth, 260, 2, 25, WHITE);
        }

        DrawText("Arquivos disponíveis na pasta do jogo:", 200, 320, 18, LIGHTGRAY);
        DrawText("- fase1.txt, fase2.txt, fase3.txt (padrão)", 220, 350, 18, GRAY);
        DrawText("- Ou crie seu próprio arquivo .txt com 24x20 caracteres", 220, 380, 18, GRAY);

        if (strlen(customLevelFile) > 0)
        {
            if (ValidateMapFile(customLevelFile))
            {
                DrawText("✓ Arquivo válido!", 220, 410, 20, GREEN);

                // Mostra primeiras linhas como preview
                FILE *preview = fopen(customLevelFile, "r");
                if (preview)
                {
                    char previewLine[50];
                    DrawText("Preview (primeiras 5 linhas):", 220, 440, 18, GRAY);
                    for (int i = 0; i < 5 && fgets(previewLine, sizeof(previewLine), preview); i++)
                    {
                        previewLine[24] = '\0';
                        DrawText(previewLine, 220, 470 + (i * 25), 18, LIGHTGRAY);
                    }
                    fclose(preview);
                }
            }
            else
            {
                DrawText("⚠ Verifique formato (24x20, bordas 'T')", 220, 410, 20, ORANGE);
            }
        }

        DrawText("Pressione ENTER para carregar", 300, 550, 20, GREEN);
        DrawText("Pressione ESC para voltar ao menu", 300, 580, 20, RED);
        break;

    case NAME_INPUT:
        ClearBackground(BLACK);
        DrawText("DIGITE SEU NOME:", 300, 300, 20, WHITE);
        DrawText(playerNameInput, 300, 350, 40, YELLOW);
        DrawText("Pressione ENTER para Salvar", 300, 500, 20, GRAY);
        break;

    case RANKING:
        ClearBackground(BLACK);
        DrawText("HIGHSCORES", 350, 100, 30, GREEN);
        for (int i = 0; i < MAX_HIGHSCORES; i++)
        {
            DrawText(TextFormat("%d. %s  -  %d", i + 1, highScores[i].name, highScores[i].score),
                     300, 160 + (i * 40), 20, WHITE);
        }
        DrawText("Pressione ENTER para Voltar", 300, 700, 20, GRAY);
        break;
    }
    EndTextureMode();
};

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "River Raid Clone - Raylib C");
    SetTargetFPS(60);

    // Define tamanho mínimo da janela
    SetWindowMinSize(480, 400); // Metade do tamanho original

    LoadHighScores();

    totalLevels = CountAvailableLevels();
    if (totalLevels == 0)
    {
        printf("AVISO: Nenhuma fase encontrada!\n");
        totalLevels = 1;
    }

    // RenderTexture para renderizar em resolução fixa
    RenderTexture2D target = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);

    // Para evitar distorção, desabilitamos o filtro de textura
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    // Loop Principal
    while (!WindowShouldClose())
    {
        // Obtém dimensões atuais da janela
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();

        // Calcula proporção para manter aspecto 960x800
        float scale = fminf((float)screenWidth / SCREEN_WIDTH,
                            (float)screenHeight / SCREEN_HEIGHT);

        // Calcula offset para centralizar
        int renderWidth = (int)(SCREEN_WIDTH * scale);
        int renderHeight = (int)(SCREEN_HEIGHT * scale);
        int offsetX = (screenWidth - renderWidth) / 2;
        int offsetY = (screenHeight - renderHeight) / 2;

        // Lógica de cada estado
        switch (currentState)
        {
        case MENU:
            // Processa input primeiro
            if (IsKeyPressed(KEY_ENTER))
            {
                isCustomLevel = 0;
                ResetGame();
                currentState = GAMEPLAY;
            }
            if (IsKeyPressed(KEY_C))
            {
                strcpy(customLevelFile, "");
                currentState = CUSTOM_LEVEL;
            }
            if (IsKeyPressed(KEY_R))
                currentState = RANKING;

            // Renderiza para a textura
            RenderToTarget(target);
            break;

        case GAMEPLAY:
            UpdateGame();

            // Renderiza o jogo na textura
            BeginTextureMode(target);
            DrawGame(); // DrawGame já tem seu próprio ClearBackground
            EndTextureMode();

            // Verifica estado do jogo
            if (!playerActive)
            {
                if (gameWon)
                {
                    currentState = VICTORY;
                }
                else
                {
                    currentState = GAME_OVER;
                }
            }
            break;

        case GAME_OVER:
            // Processa input
            if (IsHighScore(score))
            {
                if (IsKeyPressed(KEY_ENTER))
                {
                    currentState = NAME_INPUT;
                    letterCount = 0;
                    for (int i = 0; i < 20; i++)
                        playerNameInput[i] = '\0';
                }
            }
            else
            {
                if (IsKeyPressed(KEY_ENTER))
                    currentState = MENU;
            }

            // Renderiza para a textura
            RenderToTarget(target);
            break;

        case VICTORY:
            // Processa input
            if (IsHighScore(score))
            {
                if (IsKeyPressed(KEY_ENTER))
                {
                    currentState = NAME_INPUT;
                    letterCount = 0;
                    for (int i = 0; i < 20; i++)
                        playerNameInput[i] = '\0';
                }
            }
            else
            {
                if (IsKeyPressed(KEY_ENTER))
                    currentState = MENU;
            }

            // Renderiza para a textura
            RenderToTarget(target);
            break;

        case CUSTOM_LEVEL:
            // Processa input de texto
            int key = GetCharPressed();
            while (key > 0)
            {
                if (strlen(customLevelFile) < 50)
                {
                    if ((key >= 32 && key <= 126) &&
                        (key != '\\' && key != '/' && key != ':' &&
                         key != '*' && key != '?' && key != '"' &&
                         key != '<' && key != '>' && key != '|'))
                    {
                        int len = strlen(customLevelFile);
                        customLevelFile[len] = (char)key;
                        customLevelFile[len + 1] = '\0';
                    }
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE))
            {
                int len = strlen(customLevelFile);
                if (len > 0)
                    customLevelFile[len - 1] = '\0';
            }

            // Tecla Enter para carregar
            if (IsKeyPressed(KEY_ENTER) && strlen(customLevelFile) > 0)
            {
                FILE *testFile = fopen(customLevelFile, "r");
                if (testFile)
                {
                    fclose(testFile);

                    isCustomLevel = 1;
                    currentLevel = 1;
                    totalLevels = 1;

                    // Reseta o jogo
                    playerX = SCREEN_WIDTH / 2;
                    playerY = SCREEN_HEIGHT - 100;
                    fuel = 100.0f;
                    score = 0;
                    playerActive = 1;
                    scrollY = 0;
                    enemyCount = 0;

                    for (int i = 0; i < MAX_BULLETS; i++)
                        bullets[i].active = 0;

                    LoadMap(customLevelFile);
                    currentState = GAMEPLAY;
                }
                else
                {
                    printf("ERRO: Arquivo '%s' não encontrado!\n", customLevelFile);
                }
            }

            // ESC volta ao menu
            if (IsKeyPressed(KEY_ESCAPE))
            {
                currentState = MENU;
            }

            // Renderiza para a textura
            RenderToTarget(target);
            break;

        case NAME_INPUT:
            // Captura de texto
            key = GetCharPressed();
            while (key > 0)
            {
                if ((key >= 32) && (key <= 125) && (letterCount < 10))
                {
                    playerNameInput[letterCount] = (char)key;
                    playerNameInput[letterCount + 1] = '\0';
                    letterCount++;
                }
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE))
            {
                letterCount--;
                if (letterCount < 0)
                    letterCount = 0;
                playerNameInput[letterCount] = '\0';
            }

            if (IsKeyPressed(KEY_ENTER) && letterCount > 0)
            {
                AddHighScore(score, playerNameInput);
                currentState = RANKING;
            }

            // Renderiza para a textura
            RenderToTarget(target);
            break;

        case RANKING:
            if (IsKeyPressed(KEY_ENTER))
                currentState = MENU;

            // Renderiza para a textura
            RenderToTarget(target);
            break;
        }

        // Desenha a textura renderizada na tela (com redimensionamento)
        BeginDrawing();
        ClearBackground(BLACK); // Fundo preto ao redor

        // Desenha a textura centralizada mantendo proporção
        DrawTexturePro(
            target.texture,
            (Rectangle){0, 0, SCREEN_WIDTH, -SCREEN_HEIGHT},          // Área de origem
            (Rectangle){offsetX, offsetY, renderWidth, renderHeight}, // Área de destino
            (Vector2){0, 0},                                          // Origem
            0.0f,                                                     // Rotação
            WHITE                                                     // Cor
        );

        EndDrawing();
    }

    // Limpeza
    UnloadRenderTexture(target);
    CloseWindow();
    return 0;
}
