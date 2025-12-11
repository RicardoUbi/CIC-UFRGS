#include "game.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

// Nomes amigáveis para os níveis
static const char *levelNames[] = {
    "Rio Amazônico",
    "Cânion Profundo",
    "Base Militar",
    "Cidade Flutuante",
    "Fortaleza Final"};

// Textos do menu
static const char *menuOptions[] = {
    "Pressione ENTER para Campanha",
    "Pressione C para Fase Personalizada",
    "Pressione R para Ranking",
    "Pressione ESC para Sair"};

#define BACKGROUND_COLOR (Color){20, 20, 40, 255}
#define UI_BACKGROUND (Color){0, 0, 0, 200}
#define SELECTED_COLOR (Color){255, 200, 50, 255}

// Variáveis para animações
static float menuPulse = 0.0f;
static int currentMenuOption = 0;
static float menuSelectionTimer = 0.0f;

/* ---------- Helpers privados ---------- */

/*
 * ResolveMapPath:
 *  - Se 'name' já existe como arquivo, retorna o caminho direto (em buffer estático).
 *  - Senão, tenta "src/assets/maps/<name>".
 *  - Se nenhum existir, retorna o candidato "src/assets/maps/<name>" (para mensagens de erro/validação).
 *
 * Uso: chamar e usar o ponteiro retornado imediatamente (é estático).
 */
static const char *ResolveMapPath(const char *name)
{
    static char resolved[512];
    if (!name || strlen(name) == 0)
    {
        resolved[0] = '\0';
        return resolved;
    }

    // Testa caminho direto
    if (FileExists(name))
    {
        strncpy(resolved, name, sizeof(resolved) - 1);
        resolved[sizeof(resolved) - 1] = '\0';
        return resolved;
    }

    // Testa dentro da pasta de assets
    snprintf(resolved, sizeof(resolved), "src/assets/maps/%s", name);
    if (FileExists(resolved))
    {
        return resolved;
    }

    // Se nada existir, devolve candidato dentro de assets (para validar/abrir)
    return resolved;
}

/*
 * SanitizeHighScores:
 *  - Remove nomes com caracteres não imprimíveis e garante scores não-negativos.
 *  - Isso evita "dados lixo" na tela caso a leitura do arquivo tenha corrompido strings.
 */
static void SanitizeHighScores(Game *game)
{
    if (!game)
        return;

    for (int i = 0; i < MAX_HIGHSCORES; i++)
    {
        // Score mínimo 0
        if (game->highScores[i].score < 0)
            game->highScores[i].score = 0;

        // Nome deve conter pelo menos um caractere imprimível; senão substitui por '---'
        int printable = 0;
        for (size_t j = 0; j < strlen(game->highScores[i].name); j++)
        {
            if (isprint((unsigned char)game->highScores[i].name[j]))
            {
                printable = 1;
                break;
            }
        }
        if (!printable || strlen(game->highScores[i].name) == 0)
        {
            strncpy(game->highScores[i].name, "---", sizeof(game->highScores[i].name) - 1);
            game->highScores[i].name[sizeof(game->highScores[i].name) - 1] = '\0';
        }
    }
}

/* ---------- Funções públicas/fluxo ---------- */

void LoadCampaignLevel(Game *game, int level)
{
    if (!game)
        return;

    if (level < 1 || level > game->totalLevels)
    {
        printf("LoadCampaignLevel: nível inválido (%d)\n", level);
        return;
    }

    char path[256];
    snprintf(path, sizeof(path),
             "src/assets/maps/fase%d.txt",
             level);

    game->isCustomLevel = 0;
    game->currentLevel = level;

    printf("Carregando fase da campanha: %s\n", path);

    LoadLevel(game, path);
}

void LoadNextLevel(Game *game)
{
    if (!game)
        return;

    int nextLevel = game->currentLevel + 1;

    if (nextLevel > game->totalLevels)
    {
        game->gameWon = 1;
        ChangeState(game, VICTORY);
        return;
    }

    LoadCampaignLevel(game, nextLevel);
}

int CheckLevelCompletion(Game *game)
{
    if (!game)
        return 0;

    float endScroll = -(game->map.height * TILE_SIZE);

    return game->map.scrollY <= endScroll;
}

void DrawSkinPreview(Game *game)
{
    if (!game)
        return;

    Texture2D tex = game->player.texture;
    if (tex.id == 0)
        return;

    float scale = 0.7f;

    float dstW = tex.width * scale;
    float dstH = tex.height * scale;

    Rectangle src = {0, 0, tex.width, tex.height};

    Rectangle dst = {
        SCREEN_WIDTH / 2,
        600,
        dstW,
        dstH};

    Vector2 origin = {
        dstW / 2,
        0};

    DrawTexturePro(tex, src, dst, origin, 0.0f, WHITE);
}

void AddCurrentScore(Game *game)
{
    if (!game)
        return;

    if (strlen(game->playerName) == 0)
    {
        strcpy(game->playerName, "ANON");
    }

    AddHighScore(game->highScores, game->score, game->playerName);
    SaveHighScores(game->highScores);

    // After saving, sanitize to keep UI safe
    SanitizeHighScores(game);

    printf("Highscore salvo: %s - %d pontos\n", game->playerName, game->score);
}

/* ===== INICIALIZAÇÃO E RECURSOS ===== */

void LoadGameResources(Game *game)
{
    if (!game)
        return;

    // Carrega highscores
    LoadHighScores(game->highScores);
    SanitizeHighScores(game);

    printf("Recursos do jogo carregados\n");
}

void UnloadGameResources(Game *game)
{
    if (!game)
        return;

    UnloadPlayerTextures(&game->player);
    UnloadBulletSystem(&game->bulletSystem);

    printf("Recursos do jogo descarregados\n");
}

void InitGame(Game *game)
{
    if (!game)
        return;

    memset(game, 0, sizeof(Game));

    for (int i = 0; i < MAX_HIGHSCORES; i++)
    {
        game->highScores[i].score = 0;
        strncpy(game->highScores[i].name, "---", sizeof(game->highScores[i].name) - 1);
        game->highScores[i].name[sizeof(game->highScores[i].name) - 1] = '\0';
    }

    // Inicializa valores padrão
    game->currentState = MENU;
    game->score = 0;
    game->gameWon = 0;
    game->currentLevel = 1;
    game->totalLevels = TOTAL_LEVELS;
    game->isCustomLevel = 0;
    game->customLevelFile[0] = '\0';
    game->playerName[0] = '\0';
    game->nameInputPos = 0;

    game->gameTime = 0.0f;
    game->levelStartTime = 0.0f;

    game->showHitboxes = 0;
    game->godMode = 0;
    game->gameOver = 0;
    game->gameOverTimer = 0.0f;
    game->blinkTimer = 0.0f;

    game->windowWidth = SCREEN_WIDTH;
    game->windowHeight = SCREEN_HEIGHT;
    game->scaleFactor = 1.0f;

    // Inicializa sistemas
    InitPlayer(&game->player);
    InitBulletSystem(&game->bulletSystem);
    InitObstacles(&game->obstacleSystem);

    // Carrega mapa de fundo do menu (fase1 como fundo)
    LoadMap(&game->map, "src/assets/maps/fase1.txt");

    // Carrega highscores
    LoadHighScores(game->highScores);
    SanitizeHighScores(game);

    printf("Sistema de jogo inicializado\n");
    printf("Estado inicial: MENU\n");
    printf("Total de níveis: %d\n", game->totalLevels);
    printf("Tamanho da janela: %dx%d\n", game->windowWidth, game->windowHeight);
}

void ResetGame(Game *game)
{
    if (!game)
        return;

    int currentSkin = game->player.currentSkin;
    InitPlayer(&game->player);
    if (currentSkin != 0)
    {
        ChangePlayerSkin(&game->player, currentSkin);
    }

    ClearBullets(&game->bulletSystem);
    ClearObstacles(&game->obstacleSystem);

    game->score = 0;
    game->gameWon = 0;
    game->currentLevel = 1;
    game->isCustomLevel = 0;
    game->customLevelFile[0] = '\0';

    game->gameTime = 0.0f;
    game->levelStartTime = 0.0f;

    game->gameOver = 0;
    game->gameOverTimer = 0.0f;
    game->blinkTimer = 0.0f;
    if (!game->pauseType || game->player.active)
    {
        game->player.fuel = 100.0f;
    }
    printf("Jogo resetado\n");
}

/* ===== CONTROLE DE ESTADO ===== */

void ChangeState(Game *game, GameState newState)
{
    if (!game)
        return;

    printf("Mudando estado: %d -> %d\n", game->currentState, newState);
    game->currentState = newState;

    switch (newState)
    {
    case MENU:
        game->score = 0;
        game->gameWon = 0;
        game->currentLevel = 1;
        game->isCustomLevel = 0;
        currentMenuOption = 0;
        menuSelectionTimer = 0.0f;
        break;

    case GAMEPLAY:
        game->levelStartTime = GetTime();
        game->gameOver = 0;
        game->gameOverTimer = 0.0f;
        game->blinkTimer = 0.0f;
        if (!game->isCustomLevel)
        {
            LoadCampaignLevel(game, game->currentLevel);
        }
        else
        {
            LoadCustomLevel(game, game->customLevelFile);
        }
        break;

    case GAME_OVER:
        game->gameOver = 1;
        game->gameOverTimer = 0.0f;
        break;

    case VICTORY:
        game->gameWon = 1;
        break;

    case CUSTOM_LEVEL:
        game->customLevelFile[0] = '\0';
        break;

    case NAME_INPUT:
        game->playerName[0] = '\0';
        game->nameInputPos = 0;
        break;

    case HIGHSCORE:
        LoadHighScores(game->highScores);
        SanitizeHighScores(game);
        break;

    default:
        break;
    }
}

/* ===== CONTROLE DE NÍVEIS ===== */

void LoadLevel(Game *game, const char *levelFile)
{
    if (!game || !levelFile)
        return;

    printf("Carregando nível: %s\n", levelFile);

    // Limpa sistemas
    ClearBullets(&game->bulletSystem);
    ClearObstacles(&game->obstacleSystem);

    // Carrega mapa
    LoadMap(&game->map, levelFile);

    // Reseta posição de scroll para o começo do nível
    game->map.scrollY = 0.0f;

    // Carrega obstáculos do mapa
    LoadObstaclesFromMap(&game->obstacleSystem, &game->map);

    // Configurações iniciais
    game->map.scrollSpeed = 2.0f;
    game->levelStartTime = GetTime();
    game->gameOver = 0;

    // Garante jogador ativo e posicionado corretamente (centro inferior da tela)
    game->player.x = (float)(SCREEN_WIDTH / 2);
    game->player.y = (float)(SCREEN_HEIGHT - 40);
    game->player.active = 1;
    if (game->player.lives <= 0)
        game->player.lives = 3; // fallback

    printf("Obstáculos carregados: %d\n", GetActiveObstacleCount(&game->obstacleSystem));
}

void LoadCustomLevel(Game *game, const char *filename)
{
    if (!game || !filename)
        return;

    strncpy(game->customLevelFile, filename, sizeof(game->customLevelFile) - 1);
    game->customLevelFile[sizeof(game->customLevelFile) - 1] = '\0';

    game->isCustomLevel = 1;
    game->currentLevel = 0; // indica custom

    const char *resolved = ResolveMapPath(filename);

    if (!ValidateMapFile(resolved))
    {
        printf("Nível personalizado inválido (ValidateMapFile falhou): %s (resolvido: %s)\n",
               filename, resolved);
        ChangeState(game, MENU);
        return;
    }

    LoadMap(&game->map, resolved);

    game->map.scrollY = 0.0f;

    LoadObstaclesFromMap(&game->obstacleSystem, &game->map);

    game->map.scrollSpeed = 2.0f;
    game->levelStartTime = GetTime();
    game->gameOver = 0;

    game->player.x = (float)(SCREEN_WIDTH / 2);
    game->player.y = (float)(SCREEN_HEIGHT - 40);
    game->player.active = 1;

    printf("Nível personalizado carregado: %s (resolvido: %s)\n", filename, resolved);
}

/* ===== ATUALIZAÇÃO DO JOGO ===== */

void UpdateGameplay(Game *game)
{
    if (!game || !game->player.active)
        return;

    UpdateMapScroll(&game->map);
    UpdatePlayer(&game->player);
    UpdateBullets(&game->bulletSystem);

    int pointsEarned = CheckBulletObstacleCollision(&game->obstacleSystem,
                                                    &game->bulletSystem,
                                                    game->map.scrollY);
    if (pointsEarned > 0)
    {
        game->score += pointsEarned;
        printf("+%d pontos! Total: %d\n", pointsEarned, game->score);
    }

    if (!game->godMode)
    {
        HandlePlayerObstacleCollision(&game->obstacleSystem, &game->player, game->map.scrollY);
    }

    if (!game->player.active && !game->godMode)
    {
        printf("Jogador morreu! Pontuação: %d\n", game->score);
        ChangeState(game, GAME_OVER);
        return;
    }

    if (game->player.fuel <= 0 && !game->godMode)
    {
        printf("Combustível acabou! Pontuação: %d\n", game->score);
        ChangeState(game, GAME_OVER);
        return;
    }

    if (!game->gameOver && CheckLevelCompletion(game))
    {
        if (!game->isCustomLevel)
        {
            int elapsed = (int)(GetTime() - game->levelStartTime);
            int timeBonus = elapsed > 0 ? (1000 / elapsed) : 0;
            if (timeBonus > 0)
            {
                game->score += timeBonus;
                printf("Bônus de tempo: +%d pontos\n", timeBonus);
            }

            LoadNextLevel(game);
        }

        // Remover
        else
        {
            game->gameWon = 1;
            ChangeState(game, VICTORY);
        }
    }

    if (game->pauseCount > 0)
    {
        game->pauseTimer += GetFrameTime();

        if (game->pauseTimer >= 1.0f)
        {
            game->pauseTimer = 0;
            game->pauseCount--;

            if (game->pauseCount == 0)
                return; // gameplay continua no próximo frame
        }

        return;
    }

    ProcessGameplayInput(game);
}

/* ===== INPUT HANDLING ===== */

void ProcessPauseInput(Game *game)
{
    if (!game)
        return;

    // PAUSA COM CONFIRMAÇÃO (ESC)
    if (game->pauseType == 1)
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            ChangeState(game, MENU);
        }

        if (IsKeyPressed(KEY_ESCAPE))
        {
            ChangeState(game, GAMEPLAY);
        }
    }

    // PAUSA SIMPLES (ENTER)
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            game->pauseCount = 3;
            game->pauseTimer = 0;
            ChangeState(game, GAMEPLAY);
        }
    }
}

void ProcessMenuInput(Game *game)
{
    if (IsKeyPressed(KEY_UP))
    {
        currentMenuOption = (currentMenuOption - 1 + 4) % 4;
        menuSelectionTimer = 0.0f;
    }
    else if (IsKeyPressed(KEY_DOWN))
    {
        currentMenuOption = (currentMenuOption + 1) % 4;
        menuSelectionTimer = 0.0f;
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        switch (currentMenuOption)
        {
        case 0: // Campanha
            ChangeState(game, GAMEPLAY);
            break;
        case 1: // Fase Personalizada
            ChangeState(game, CUSTOM_LEVEL);
            break;
        case 2: // Ranking
            ChangeState(game, HIGHSCORE);
            break;
        case 3: // Sair
            printf("Saindo do jogo (menu)...\n");
            CloseWindow();
            break;
        }
    }
    else if (IsKeyPressed(KEY_C))
    {
        ChangeState(game, CUSTOM_LEVEL);
    }
    else if (IsKeyPressed(KEY_R))
    {
        ChangeState(game, HIGHSCORE);
    }
    else if (IsKeyPressed(KEY_ESCAPE))
    {
        // Fecha a janela quando ESC no menu
        printf("Saindo do jogo (ESC)...\n");
        CloseWindow();
    }
    else if (IsKeyPressed(KEY_HOME) || IsKeyPressed(KEY_F11))
    {
        PreviousPlayerSkin(&game->player);
    }
    else if (IsKeyPressed(KEY_END) || IsKeyPressed(KEY_F12))
    {
        NextPlayerSkin(&game->player);
    }
}

void ProcessGameplayInput(Game *game)
{
    if (!game)
        return;

    MovePlayer(&game->player);

    if (IsKeyPressed(KEY_SPACE) || IsKeyDown(KEY_K))
    {
        FireBulletFromPlayer(&game->bulletSystem, &game->player);
    }

    if (IsKeyPressed(KEY_F))
    {
        Refuel(&game->player, 30.0f);
        printf("Reabastecido! Combustível: %.0f\n", game->player.fuel);
    }

    if (IsKeyPressed(KEY_H))
    {
        game->showHitboxes = !game->showHitboxes;
        printf("Hitboxes: %s\n", game->showHitboxes ? "ON" : "OFF");
    }

    if (IsKeyPressed(KEY_I))
    {
        game->godMode = !game->godMode;
        game->player.invincible = game->godMode;
        game->player.invincibleTimer = game->godMode ? 9999.0f : 0;
        printf("God mode: %s\n", game->godMode ? "ON" : "OFF");
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        game->pauseType = 1;
        ChangeState(game, GAME_PAUSED);
    }

    if (IsKeyPressed(KEY_R))
    {
        if (!game->isCustomLevel)
            LoadCampaignLevel(game, game->currentLevel);
        else
            LoadCustomLevel(game, game->customLevelFile);
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        game->pauseType = 0; // pausa normal
        ChangeState(game, GAME_PAUSED);
    }
}

void ProcessGameOverInput(Game *game)
{
    if (IsKeyPressed(KEY_ENTER))
    {
        if (IsHighScore(game->highScores, game->score) && !game->isCustomLevel)
        {
            ChangeState(game, NAME_INPUT);
        }
        else
        {
            ChangeState(game, MENU);
        }
    }
    else if (IsKeyPressed(KEY_R))
    {
        if (!game->isCustomLevel)
        {
            LoadCampaignLevel(game, game->currentLevel);
        }
        else
        {
            LoadCustomLevel(game, game->customLevelFile);
        }
        ChangeState(game, GAMEPLAY);
    }
    else if (IsKeyPressed(KEY_ESCAPE))
    {
        ChangeState(game, MENU);
    }
}

void ProcessVictoryInput(Game *game)
{
    if (IsKeyPressed(KEY_ENTER))
    {
        if (IsHighScore(game->highScores, game->score) && !game->isCustomLevel)
        {
            ChangeState(game, NAME_INPUT);
        }
        else if (!game->isCustomLevel && game->currentLevel <= TOTAL_LEVELS)
        {
            LoadNextLevel(game);
        }
        else
        {
            ChangeState(game, MENU);
        }
    }
    else if (IsKeyPressed(KEY_ESCAPE))
    {
        ChangeState(game, MENU);
    }
}

void ProcessCustomLevelInput(Game *game)
{
    if (IsKeyPressed(KEY_ESCAPE))
    {
        ChangeState(game, MENU);
        return;
    }

    int key = GetCharPressed();
    while (key > 0)
    {
        if (strlen(game->customLevelFile) < sizeof(game->customLevelFile) - 1)
        {
            if ((key >= 'A' && key <= 'Z') || (key >= 'a' && key <= 'z') ||
                (key >= '0' && key <= '9') || key == '.' || key == '_' || key == '-' ||
                key == '/' || key == '\\')
            {
                int len = strlen(game->customLevelFile);
                game->customLevelFile[len] = (char)key;
                game->customLevelFile[len + 1] = '\0';
            }
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE))
    {
        int len = strlen(game->customLevelFile);
        if (len > 0)
        {
            game->customLevelFile[len - 1] = '\0';
        }
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        if (strlen(game->customLevelFile) > 0)
        {
            game->isCustomLevel = 1;
            ChangeState(game, GAMEPLAY);
        }
    }
}

void ProcessNameInput(Game *game)
{
    int key = GetCharPressed();
    while (key > 0)
    {
        if (game->nameInputPos < MAX_NAME_LENGTH - 1)
        {
            if (key >= 'a' && key <= 'z')
                key -= 32;

            if (key >= 'A' && key <= 'Z')
            {
                game->playerName[game->nameInputPos] = (char)key;
                game->nameInputPos++;
                game->playerName[game->nameInputPos] = '\0';
            }
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && game->nameInputPos > 0)
    {
        game->nameInputPos--;
        game->playerName[game->nameInputPos] = '\0';
    }

    if (IsKeyPressed(KEY_ENTER) && game->nameInputPos > 0)
    {
        AddCurrentScore(game);
        ChangeState(game, MENU);
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        ChangeState(game, MENU);
    }
}

void ProcessHighScoreInput(Game *game)
{
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_SPACE))
    {
        ChangeState(game, MENU);
    }
}

void UpdateGame(Game *game)
{
    if (!game)
        return;

    float deltaTime = GetFrameTime();
    game->gameTime += deltaTime;

    if (game->currentState == MENU || game->currentState == CUSTOM_LEVEL)
    {
        menuPulse += deltaTime * 2.0f;
        menuSelectionTimer += deltaTime;
    }

    switch (game->currentState)
    {
    case MENU:
        ProcessMenuInput(game);
        break;

    case GAMEPLAY:
        UpdateGameplay(game);
        break;

    case GAME_PAUSED:
        ProcessPauseInput(game);
        break;

    case GAME_OVER:
        ProcessGameOverInput(game);
        game->gameOverTimer += deltaTime;
        game->blinkTimer += deltaTime;
        break;

    case VICTORY:
        ProcessVictoryInput(game);
        break;

    case CUSTOM_LEVEL:
        ProcessCustomLevelInput(game);
        break;

    case NAME_INPUT:
        ProcessNameInput(game);
        break;

    case HIGHSCORE:
        ProcessHighScoreInput(game);
        break;

    default:
        break;
    }
}

/* ===== RENDERIZAÇÃO ===== */

void UpdateWindowSize(Game *game, int width, int height)
{
    if (!game)
        return;

    game->windowWidth = width;
    game->windowHeight = height;

    float scaleX = (float)width / SCREEN_WIDTH;
    float scaleY = (float)height / SCREEN_HEIGHT;
    game->scaleFactor = (scaleX < scaleY) ? scaleX : scaleY;

    if (game->scaleFactor < 0.75f)
        game->scaleFactor = 0.75f;

    if (game->scaleFactor > 2.0f)
        game->scaleFactor = 2.0f;

    printf("Janela redimensionada: %dx%d (Escala: %.2f)\n",
           width, height, game->scaleFactor);
}

void DrawScaled(Game *game, void (*drawFunction)(Game *))
{
    if (!game || !drawFunction)
        return;

    int renderWidth = SCREEN_WIDTH * game->scaleFactor;
    int renderHeight = SCREEN_HEIGHT * game->scaleFactor;

    int offsetX = (game->windowWidth - renderWidth) / 2;
    int offsetY = (game->windowHeight - renderHeight) / 2;

    static RenderTexture2D target = {0};
    if (target.id == 0)
    {
        target = LoadRenderTexture(SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    BeginTextureMode(target);
    {
        ClearBackground(BLANK);
        drawFunction(game);
    }
    EndTextureMode();

    DrawTexturePro(
        target.texture,
        (Rectangle){0, 0, SCREEN_WIDTH, -SCREEN_HEIGHT},
        (Rectangle){offsetX, offsetY, renderWidth, renderHeight},
        (Vector2){0, 0},
        0.0f,
        WHITE);

    DrawRectangle(0, 0, game->windowWidth, offsetY, BLACK);                      // topo
    DrawRectangle(0, offsetY + renderHeight, game->windowWidth, offsetY, BLACK); // base
    DrawRectangle(0, offsetY, offsetX, renderHeight, BLACK);                     // esquerda
    DrawRectangle(offsetX + renderWidth, offsetY, offsetX, renderHeight, BLACK); // direita
}

/* Funções de desenho específicas para cada estado */
void DrawExitPopup(Game *game)
{
    int boxWidth = 520;
    int boxHeight = 140;
    int boxX = 260;
    int boxY = 320;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(BLACK, 0.75f));

    // Texto
    DrawText("Deseja voltar ao menu?",
             boxX + 70, boxY + 30, 30, RED);

    DrawText("ENTER - Sim | ESC - Nao",
             boxX + 120, boxY + 80, 20, GOLD);
}

void DrawPausedText(Game *game)
{
    int boxWidth = 520;
    int boxHeight = 140;
    int boxX = 260;
    int boxY = 320;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, Fade(BLACK, 0.75f));

    // Texto
    DrawText("PAUSADO",
             boxX + 180, boxY + 30, 30, GRAY);

    DrawText("ENTER para continuar",
             boxX + 130, boxY + 80, 20, GOLD);
}

void DrawPlayerWithEffects(Game *game)
{
    if (!game->player.active && game->gameOver)
    {
        // Calcula dimensões escaladas
        float width = game->player.texture.width * game->player.scale;
        float height = game->player.texture.height * game->player.scale;

        // Posição para centralizar
        float drawX = game->player.x - width / 2;
        float drawY = game->player.y - height / 2;

        // Desenha o jogador com tom vermelho
        DrawTextureEx(game->player.texture,
                      (Vector2){drawX, drawY},
                      0.0f,
                      game->player.scale,
                      (Color){255, 100, 100, 200});

        // Borda vermelha
        // DrawRectangleLinesEx((Rectangle){drawX, drawY, width, height},
        //                      3, RED);
    }
}

void DrawGameplayContent(Game *game)
{
    if (!game)
        return;

    ClearBackground(BLUE);

    int startY = (int)(game->map.scrollY / TILE_SIZE);
    int endY = startY + (SCREEN_HEIGHT / TILE_SIZE) + 1;
    if (endY > game->map.height)
        endY = game->map.height;

    for (int y = startY; y < endY; y++)
    {
        for (int x = 0; x < MAP_COLS; x++)
        {
            if (game->map.data[y][x] == 'T')
            {
                float screenY = (y * TILE_SIZE) - game->map.scrollY;
                DrawRectangle(x * TILE_SIZE, screenY, TILE_SIZE, TILE_SIZE, GREEN);
            }
        }
    }

    DrawObstacles(&game->obstacleSystem, game->map.scrollY);
    DrawBullets(&game->bulletSystem);
    DrawPlayer(&game->player);

    if (game->showHitboxes)
    {
        Rectangle playerHitbox = GetPlayerHitbox(&game->player);
        DrawRectangleLinesEx(playerHitbox, 2, RED);
    }
}

void DrawHUD(Game *game)
{
    if (!game)
        return;

    DrawRectangle(0, 0, game->windowWidth, 70, UI_BACKGROUND);
    DrawText(TextFormat("SCORE: %06d", game->score), 20, 10, 24, WHITE);

    int fuelBarWidth = 200;
    int fuelBarX = 230;
    int fuelBarY = 10;

    DrawText("FUEL:", fuelBarX, fuelBarY, 20, WHITE);
    DrawRectangle(fuelBarX + 50, fuelBarY, fuelBarWidth, 20, RED);
    DrawRectangle(fuelBarX + 50, fuelBarY, (int)(game->player.fuel * 2), 20, GREEN);
    DrawText(TextFormat("%.0f", game->player.fuel), fuelBarX + 50 + fuelBarWidth + 10, fuelBarY, 20, WHITE);

    int infoX = 550;
    if (!game->isCustomLevel)
    {
        DrawText(TextFormat("LEVEL: %d/%d", game->currentLevel, TOTAL_LEVELS),
                 infoX, 10, 20, YELLOW);
        DrawText(levelNames[game->currentLevel - 1], infoX, 35, 16, LIGHTGRAY);
    }
    else
    {
        DrawText("CUSTOM LEVEL", infoX, 10, 20, YELLOW);
    }

    DrawText(TextFormat("TIME: %.1f", GetTime() - game->levelStartTime),
             infoX + 200, 10, 18, WHITE);

    if (game->godMode)
    {
        DrawText("GOD MODE", game->windowWidth - 150, 10, 20, YELLOW);
    }

    if (game->showHitboxes)
    {
        DrawText("HITBOXES ON", game->windowWidth - 150, 35, 16, GREEN);
    }

    DrawText("ESC: Menu  R: Restart  H: Hitboxes  I: God Mode",
             game->windowWidth / 2 - 200, SCREEN_HEIGHT - 30, 16, GRAY);
}

void DrawMenuState(Game *game)
{
    ClearBackground(BACKGROUND_COLOR);

    int centerX = SCREEN_WIDTH / 2;

    int titleSize = 50;
    const char *title = "RIVER RAID INF";

    int titleWidth = MeasureText(title, titleSize);
    DrawText(title, centerX - titleWidth / 2, 80, titleSize, GREEN);

    const char *subtitle = "Ricardo Ubirajara - Algoritmos & Programação";
    DrawText(subtitle,
             centerX - MeasureText(subtitle, 20) / 2,
             160, 20, LIGHTGRAY);

    int startY = 250;
    int optionSpacing = 50;

    for (int i = 0; i < 4; i++)
    {
        Color color = WHITE;
        int fontSize = 24;

        if (i == currentMenuOption)
        {
            fontSize = 28;
            color = SELECTED_COLOR;
        }

        int textWidth = MeasureText(menuOptions[i], fontSize);
        DrawText(menuOptions[i],
                 centerX - textWidth / 2,
                 startY + i * optionSpacing,
                 fontSize,
                 color);
    }

    // Highscore
    const char *hsText =
        TextFormat("HIGHSCORE: %s - %d",
                   game->highScores[0].name,
                   game->highScores[0].score);

    DrawText(hsText,
             centerX - MeasureText(hsText, 20) / 2,
             490, 20, GOLD);

    // Skin info
    const char *skinText =
        TextFormat("SKIN: %d/%d",
                   game->player.currentSkin + 1,
                   GetPlayerSkinCount());

    DrawText(skinText,
             centerX - MeasureText(skinText, 20) / 2,
             540, 20, YELLOW);

    const char *hintText = "Use F11/F12 ou Home/End para mudar";
    DrawText(hintText,
             centerX - MeasureText(hintText, 16) / 2,
             570, 16, GRAY);

    DrawSkinPreview(game);

    const char *exitText = "Pressione ESC para sair";
    DrawText(exitText,
             centerX - MeasureText(exitText, 18) / 2,
             SCREEN_HEIGHT - 50,
             18, DARKGRAY);
}

void DrawGameplayState(Game *game)
{
    DrawScaled(game, DrawGameplayContent);
    DrawHUD(game);

    if (game->gameOver)
    {
        DrawGameOverState(game);
    }
}

void DrawGameOverState(Game *game)
{
    DrawRectangle(0, 0, game->windowWidth, game->windowHeight, (Color){0, 0, 0, 180});

    if (((int)(game->blinkTimer * 3) % 2) == 0)
    {
        DrawText("GAME OVER",
                 game->windowWidth / 2 - MeasureText("GAME OVER", 80) / 2,
                 game->windowHeight / 2 - 100, 80, RED);

        DrawPlayerWithEffects(game);
    }

    DrawText(TextFormat("SCORE: %06d", game->score),
             game->windowWidth / 2 - MeasureText(TextFormat("SCORE: %06d", game->score), 40) / 2,
             game->windowHeight / 2, 40, WHITE);

    if (IsHighScore(game->highScores, game->score) && !game->isCustomLevel)
    {
        DrawText("NOVO RECORDE!",
                 game->windowWidth / 2 - MeasureText("NOVO RECORDE!", 30) / 2,
                 game->windowHeight / 2 + 60, 30, YELLOW);
        DrawText("Pressione ENTER para salvar",
                 game->windowWidth / 2 - MeasureText("Pressione ENTER para salvar", 20) / 2,
                 game->windowHeight / 2 + 100, 20, GREEN);
    }
    else if (!game->isCustomLevel)
    {
        DrawText(TextFormat("Recorde: %d", game->highScores[0].score),
                 game->windowWidth / 2 - MeasureText(TextFormat("Recorde: %d", game->highScores[0].score), 30) / 2,
                 game->windowHeight / 2 + 60, 30, GRAY);
    }

    DrawText("Pressione R para tentar novamente",
             game->windowWidth / 2 - MeasureText("Pressione R para tentar novamente", 20) / 2,
             game->windowHeight / 2 + 140, 20, WHITE);
    DrawText("Pressione ESC para voltar ao menu",
             game->windowWidth / 2 - MeasureText("Pressione ESC para voltar ao menu", 20) / 2,
             game->windowHeight / 2 + 170, 20, WHITE);

    if (((int)(game->blinkTimer * 3) % 2) == 0)
    {
        if (game->gameOver && game->player.fuel <= 0)
        {
            DrawText("Motivo de derrota: sem combustível",
                     game->windowWidth / 2 - MeasureText("Motivo de derrota: sem combustível", 20) / 2,
                     game->windowHeight / 2 + 230, 20, RED);
        }
        if (game->gameOver && game->player.fuel > 0 && game->player.active == 0)
        {
            DrawText("Motivo de derrota: colisão fatal",
                     game->windowWidth / 2 - MeasureText("Motivo de derrota: sem combustível", 20) / 2,
                     game->windowHeight / 2 + 230, 20, RED);
        }
    }
}

void DrawVictoryState(Game *game)
{
    DrawGameplayContent(game);

    DrawRectangle(0, 0, game->windowWidth, game->windowHeight, (Color){0, 0, 0, 180});

    DrawText("VITÓRIA!",
             game->windowWidth / 2 - MeasureText("VITÓRIA!", 80) / 2,
             game->windowHeight / 2 - 150, 80, GREEN);

    if (!game->isCustomLevel)
    {
        DrawText(TextFormat("Nível %d completado!", game->currentLevel),
                 game->windowWidth / 2 - MeasureText(TextFormat("Nível %d completado!", game->currentLevel), 40) / 2,
                 game->windowHeight / 2 - 50, 40, YELLOW);
    }
    else
    {
        DrawText("Nível personalizado completado!",
                 game->windowWidth / 2 - MeasureText("Nível personalizado completado!", 40) / 2,
                 game->windowHeight / 2 - 50, 40, YELLOW);
    }

    DrawText(TextFormat("SCORE: %06d", game->score),
             game->windowWidth / 2 - MeasureText(TextFormat("SCORE: %06d", game->score), 40) / 2,
             game->windowHeight / 2 + 20, 40, WHITE);

    if (IsHighScore(game->highScores, game->score) && !game->isCustomLevel)
    {
        DrawText("NOVO RECORDE!",
                 game->windowWidth / 2 - MeasureText("NOVO RECORDE!", 30) / 2,
                 game->windowHeight / 2 + 80, 30, YELLOW);
        DrawText("Pressione ENTER para salvar",
                 game->windowWidth / 2 - MeasureText("Pressione ENTER para salvar", 20) / 2,
                 game->windowHeight / 2 + 120, 20, GREEN);
    }

    DrawText("Pressione ESC para voltar ao menu",
             game->windowWidth / 2 - MeasureText("Pressione ESC para voltar ao menu", 20) / 2,
             game->windowHeight / 2 + 190, 20, WHITE);
}

void DrawCustomLevelState(Game *game)
{
    ClearBackground(BACKGROUND_COLOR);

    DrawText("MODO FASE PERSONALIZADA",
             game->windowWidth / 2 - MeasureText("MODO FASE PERSONALIZADA", 40) / 2,
             50, 40, YELLOW);

    DrawText("Digite o nome do arquivo (ex: minha_fase.txt):",
             game->windowWidth / 2 - MeasureText("Digite o nome do arquivo (ex: minha_fase.txt):", 20) / 2,
             150, 20, WHITE);

    int boxWidth = 500;
    int boxHeight = 40;
    int boxX = game->windowWidth / 2 - boxWidth / 2;
    int boxY = 200;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, DARKGRAY);
    DrawText(game->customLevelFile, boxX + 10, boxY + 10, 25, WHITE);

    if (((int)(GetTime() * 2) % 2) == 0)
    {
        int textWidth = MeasureText(game->customLevelFile, 25);
        DrawRectangle(boxX + 10 + textWidth, boxY + 10, 2, 25, WHITE);
    }

    if (strlen(game->customLevelFile) > 0)
    {
        const char *resolved = ResolveMapPath(game->customLevelFile);
        if (ValidateMapFile(resolved) && FileExists(resolved))
        {
            DrawText("Arquivo válido!",
                     game->windowWidth / 2 - MeasureText("Arquivo válido!", 20) / 2,
                     260, 20, GREEN);
        }
        else
        {
            DrawText("Arquivo inválido ou não encontrado",
                     game->windowWidth / 2 - MeasureText("Arquivo inválido ou não encontrado", 20) / 2,
                     260, 20, ORANGE);
        }
    }

    DrawText("Exemplos: fase1.txt, fase2.txt, minha_fase.txt",
             game->windowWidth / 2 - MeasureText("Exemplos: fase1.txt, fase2.txt, minha_fase.txt", 18) / 2,
             300, 18, LIGHTGRAY);

    const char *previewResolved = ResolveMapPath(game->customLevelFile);
    if (FileExists(previewResolved) && strlen(game->customLevelFile) > 0)
    {
        DrawText("Prévia do arquivo:", boxX + 170, 450, 18, LIGHTGRAY);

        FILE *f = fopen(previewResolved, "r");
        if (f)
        {
            char line[256];
            int y = 480;
            for (int i = 0; i < 5 && fgets(line, sizeof(line), f); i++)
            {
                // Remove newline para desenho limpo
                size_t ln = strlen(line);
                if (ln > 0 && (line[ln - 1] == '\n' || line[ln - 1] == '\r'))
                    line[ln - 1] = '\0';
                DrawText(line, boxX + 100, y, 18, WHITE);
                y += 22;
            }
            fclose(f);
        }
    }

    DrawText("Pressione ENTER para carregar",
             game->windowWidth / 2 - MeasureText("Pressione ENTER para carregar", 20) / 2,
             350, 20, GREEN);
    DrawText("Pressione ESC para voltar ao menu",
             game->windowWidth / 2 - MeasureText("Pressione ESC para voltar ao menu", 20) / 2,
             380, 20, WHITE);
    DrawText("Pressione BACKSPACE para apagar",
             game->windowWidth / 2 - MeasureText("Pressione BACKSPACE para apagar", 18) / 2,
             410, 18, GRAY);
}

void DrawNameInputState(Game *game)
{
    ClearBackground(BACKGROUND_COLOR);

    DrawText("NOVO RECORDE!",
             game->windowWidth / 2 - MeasureText("NOVO RECORDE!", 50) / 2,
             100, 50, YELLOW);

    DrawText(TextFormat("Pontuação: %06d", game->score),
             game->windowWidth / 2 - MeasureText(TextFormat("Pontuação: %06d", game->score), 30) / 2,
             180, 30, WHITE);

    DrawText(TextFormat("Digite seu nome (máximo %d letras):", MAX_NAME_LENGTH - 1),
             game->windowWidth / 2 - MeasureText(TextFormat("Digite seu nome (máximo %d letras):", MAX_NAME_LENGTH - 1), 25) / 2,
             250, 25, LIGHTGRAY);

    int boxWidth = 200;
    int boxHeight = 60;
    int boxX = game->windowWidth / 2 - boxWidth / 2;
    int boxY = 300;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, DARKGRAY);
    DrawText(game->playerName,
             game->windowWidth / 2 - MeasureText(game->playerName, 40) / 2,
             boxY + 10, 40, WHITE);

    if (((int)(GetTime() * 2) % 2) == 0 && game->nameInputPos < 3)
    {
        int textWidth = MeasureText(game->playerName, 40);
        DrawRectangle(game->windowWidth / 2 - MeasureText(game->playerName, 40) / 2 + textWidth,
                      boxY + 10, 2, 40, WHITE);
    }

    DrawText("Use letras de A a Z",
             game->windowWidth / 2 - MeasureText("Use letras de A a Z", 20) / 2,
             400, 20, GRAY);
    DrawText("Pressione ENTER para confirmar",
             game->windowWidth / 2 - MeasureText("Pressione ENTER para confirmar", 20) / 2,
             440, 20, GREEN);
    DrawText("Pressione ESC para cancelar",
             game->windowWidth / 2 - MeasureText("Pressione ESC para cancelar", 20) / 2,
             470, 20, WHITE);
}

void DrawHighScoreState(Game *game)
{
    ClearBackground(BACKGROUND_COLOR);

    DrawText("HIGHSCORES",
             game->windowWidth / 2 - MeasureText("HIGHSCORES", 60) / 2,
             50, 60, GREEN);

    int startX = game->windowWidth / 2 - 200;
    int startY = 150;
    int lineHeight = 40;

    DrawText("POS", startX, startY, 25, YELLOW);
    DrawText("NOME", startX + 100, startY, 25, YELLOW);
    DrawText("PONTUAÇÃO", startX + 300, startY, 25, YELLOW);

    startY += lineHeight;

    // Assegura que highScores foram saneados
    SanitizeHighScores(game);

    for (int i = 0; i < MAX_HIGHSCORES; i++)
    {
        Color color = WHITE;
        if (i == 0)
            color = GOLD;
        else if (i == 1)
            color = GRAY;
        else if (i == 2)
            color = (Color){205, 127, 50, 255};

        DrawText(TextFormat("%02d.", i + 1), startX, startY + i * lineHeight, 20, color);
        DrawText(game->highScores[i].name, startX + 100, startY + i * lineHeight, 20, color);
        DrawText(TextFormat("%08d", game->highScores[i].score),
                 startX + 300, startY + i * lineHeight, 20, color);
    }

    DrawText("Pressione ENTER ou ESC para voltar ao menu",
             game->windowWidth / 2 - MeasureText("Pressione ENTER ou ESC para voltar ao menu", 20) / 2,
             600, 20, WHITE);
}

void DrawGame(Game *game)
{
    if (!game)
        return;

    switch (game->currentState)
    {
    case MENU:
        DrawScaled(game, DrawMenuState);
        break;
    case GAMEPLAY:
        DrawGameplayState(game);
        break;

    case GAME_PAUSED:
        DrawScaled(game, DrawGameplayContent);

        if (game->pauseType == 1)
            DrawScaled(game, DrawExitPopup);
        else
            DrawScaled(game, DrawPausedText);
        break;

    case GAME_OVER:
        DrawScaled(game, DrawGameplayContent);
        DrawGameOverState(game);
        break;
    case VICTORY:
        DrawVictoryState(game); // ok
        break;
    case CUSTOM_LEVEL:
        DrawCustomLevelState(game);
        break;
    case NAME_INPUT:
        DrawNameInputState(game);
        break;
    case HIGHSCORE:
        DrawHighScoreState(game);
        break;
    default:
        break;
    }
}
