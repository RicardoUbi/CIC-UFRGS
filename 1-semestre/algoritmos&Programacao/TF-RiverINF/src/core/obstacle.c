// src/core/obstacle.c
#include "obstacle.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>

// Tamanho padrão dos obstáculos
#define OBSTACLE_SIZE TILE_SIZE

// Arrays estáticos para armazenar dados dos obstáculos
// (como Entity só tem tipo char, precisamos arrays extras)
static int obstacleHealth[MAX_ENEMIES];
static int obstacleScore[MAX_ENEMIES];
static int obstacleWidth[MAX_ENEMIES];
static int obstacleHeight[MAX_ENEMIES];
static ObstacleType obstacleTypes[MAX_ENEMIES];
static Texture2D obstacleTextures[OBSTACLE_TYPE_PONTE + 1];
// Cores para cada tipo de obstáculo
static const Color obstacleColors[] = {
    [OBSTACLE_TYPE_TERRA] = GREEN,
    [OBSTACLE_TYPE_NAVIO] = DARKBLUE,
    [OBSTACLE_TYPE_HELICOPTERO] = ORANGE,
    [OBSTACLE_TYPE_GAS] = RED,
    [OBSTACLE_TYPE_PONTE] = BROWN};

// Nomes para debug
static const char *obstacleNames[] = {
    [OBSTACLE_TYPE_NONE] = "Nenhum",
    [OBSTACLE_TYPE_TERRA] = "Terra",
    [OBSTACLE_TYPE_NAVIO] = "Navio",
    [OBSTACLE_TYPE_HELICOPTERO] = "Helicoptero",
    [OBSTACLE_TYPE_GAS] = "Gasolina",
    [OBSTACLE_TYPE_PONTE] = "Ponte"};

// Valores em pontos
static const int obstacleTypeScores[] = {
    [OBSTACLE_TYPE_NONE] = 0,
    [OBSTACLE_TYPE_TERRA] = 0,
    [OBSTACLE_TYPE_NAVIO] = 30,
    [OBSTACLE_TYPE_HELICOPTERO] = 60,
    [OBSTACLE_TYPE_GAS] = 50,
    [OBSTACLE_TYPE_PONTE] = 200};

// Vida inicial de cada tipo
static const int obstacleTypeHealth[] = {
    [OBSTACLE_TYPE_NONE] = 0,
    [OBSTACLE_TYPE_TERRA] = 999,
    [OBSTACLE_TYPE_NAVIO] = 2,
    [OBSTACLE_TYPE_HELICOPTERO] = 2,
    [OBSTACLE_TYPE_GAS] = 2,
    [OBSTACLE_TYPE_PONTE] = 2};

// Largura em tiles de cada tipo
static const int obstacleTypeWidth[] = {
    [OBSTACLE_TYPE_NONE] = 0,
    [OBSTACLE_TYPE_TERRA] = 1,
    [OBSTACLE_TYPE_NAVIO] = 1,
    [OBSTACLE_TYPE_HELICOPTERO] = 1,
    [OBSTACLE_TYPE_GAS] = 4,
    [OBSTACLE_TYPE_PONTE] = 4};

// Funções auxiliares internas
static char ObstacleTypeToChar(ObstacleType type)
{
    switch (type)
    {
    case OBSTACLE_TYPE_TERRA:
        return 'T';
    case OBSTACLE_TYPE_NAVIO:
        return 'N';
    case OBSTACLE_TYPE_HELICOPTERO:
        return 'X';
    case OBSTACLE_TYPE_GAS:
        return 'G';
    case OBSTACLE_TYPE_PONTE:
        return 'P';
    default:
        return ' ';
    }
}

static ObstacleType CharToObstacleType(char c)
{
    switch (c)
    {
    case 'T':
        return OBSTACLE_TYPE_TERRA;
    case 'N':
        return OBSTACLE_TYPE_NAVIO;
    case 'X':
        return OBSTACLE_TYPE_HELICOPTERO;
    case 'G':
        return OBSTACLE_TYPE_GAS;
    case 'P':
        return OBSTACLE_TYPE_PONTE;
    default:
        return OBSTACLE_TYPE_NONE;
    }
}

static void LoadObstacleTexturesInternal()
{
    // Caminhos para as texturas - ajuste conforme sua estrutura
    const char *texturePaths[] = {
        [OBSTACLE_TYPE_TERRA] = "src/assets/obstacles/terrain.png",
        [OBSTACLE_TYPE_NAVIO] = "src/assets/obstacles/ship.png",
        [OBSTACLE_TYPE_HELICOPTERO] = "src/assets/obstacles/helicopter.png",
        [OBSTACLE_TYPE_GAS] = "src/assets/obstacles/gas.png",
        [OBSTACLE_TYPE_PONTE] = "src/assets/obstacles/ponte.png"};

    for (int i = OBSTACLE_TYPE_TERRA; i <= OBSTACLE_TYPE_PONTE; i++)
    {
        if (FileExists(texturePaths[i]))
        {
            obstacleTextures[i] = LoadTexture(texturePaths[i]);
            printf("Textura carregada: %s\n", texturePaths[i]);
        }
        else
        {
            printf("AVISO: Textura não encontrada: %s\n", texturePaths[i]);
            obstacleTextures[i].id = 0; // Marca como não carregada
        }
    }
}

// Converte coordenadas de mundo para tela
static void WorldToScreen(float worldX, float worldY, float scrollY,
                          float *screenX, float *screenY)
{
    *screenX = worldX;
    *screenY = worldY - scrollY;
}

void InitObstacles(ObstacleSystem *os)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        os->entities[i].active = 0;
        os->entities[i].x = 0;
        os->entities[i].y = 0;
        os->entities[i].type = '?';

        obstacleHealth[i] = 0;
        obstacleScore[i] = 0;
        obstacleWidth[i] = OBSTACLE_SIZE;
        obstacleHeight[i] = OBSTACLE_SIZE;
        obstacleTypes[i] = OBSTACLE_TYPE_NONE;
    }

    os->activeEntitiesCount = 0;

    static int texturesLoaded = 0;
    if (!texturesLoaded)
    {
        LoadObstacleTexturesInternal();
        texturesLoaded = 1;
    }

    printf("Sistema de obstáculos inicializado\n");
    printf("Capacidade: %d obstáculos\n", MAX_ENEMIES);
}

void AddObstacle(ObstacleSystem *os, float x, float y, ObstacleType type)
{
    if (type <= OBSTACLE_TYPE_NONE || type > OBSTACLE_TYPE_PONTE)
        return;

    // Encontra slot livre
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!os->entities[i].active)
        {
            os->entities[i].active = 1;
            os->entities[i].x = x;
            os->entities[i].y = y;
            os->entities[i].type = ObstacleTypeToChar(type);

            // Armazena dados extras
            obstacleTypes[i] = type;
            obstacleHealth[i] = obstacleTypeHealth[type];
            obstacleScore[i] = obstacleTypeScores[type];
            obstacleWidth[i] = obstacleTypeWidth[type] * TILE_SIZE;
            obstacleHeight[i] = TILE_SIZE;

            os->activeEntitiesCount++;

            printf("Obstáculo %s adicionado em (%.0f, %.0f)\n",
                   obstacleNames[type], x, y);
            return;
        }
    }

    printf("Não há slots livres para obstáculos! (MAX: %d)\n", MAX_ENEMIES);
}

void UnloadObstacleTextures()
{
    for (int i = OBSTACLE_TYPE_TERRA; i <= OBSTACLE_TYPE_PONTE; i++)
    {
        if (obstacleTextures[i].id != 0)
        {
            UnloadTexture(obstacleTextures[i]);
        }
    }
}

void DrawObstacles(const ObstacleSystem *os, float scrollY)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!os->entities[i].active)
            continue;

        const Entity *ent = &os->entities[i];
        ObstacleType type = obstacleTypes[i];

        if (type <= OBSTACLE_TYPE_NONE)
            continue;

        float screenX, screenY;
        WorldToScreen(ent->x, ent->y, scrollY, &screenX, &screenY);

        if (screenY < -obstacleHeight[i] || screenY > SCREEN_HEIGHT + obstacleHeight[i])
            continue;

        // Tenta usar textura
        if (obstacleTextures[type].id != 0)
        {
            Texture2D tex = obstacleTextures[type];

            // Para obstáculos multi-tile, ajusta a escala
            float drawWidth = obstacleWidth[i];
            float drawHeight = obstacleHeight[i];

            // Calcula escala para manter proporção
            float scaleX = drawWidth / tex.width;
            float scaleY = drawHeight / tex.height;

            DrawTextureEx(tex,
                          (Vector2){screenX, screenY},
                          0.0f,
                          scaleX > scaleY ? scaleX : scaleY, // Mantém proporção
                          WHITE);
        }
        else
        {
            // Fallback: retângulo colorido
            DrawRectangle(screenX, screenY,
                          obstacleWidth[i], obstacleHeight[i],
                          obstacleColors[type]);

            // Adiciona o caractere identificador
            char typeChar = ent->type;
            int fontSize = 20;
            int textWidth = MeasureText(&typeChar, fontSize);
            DrawText(&typeChar,
                     screenX + (obstacleWidth[i] - textWidth) / 2,
                     screenY + (obstacleHeight[i] - fontSize) / 2,
                     fontSize, BLACK);
        }
    }
}

void ClearObstacles(ObstacleSystem *os)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        os->entities[i].active = 0;
    }
    os->activeEntitiesCount = 0;
    printf("Todos os obstáculos removidos\n");
}

Entity *GetObstacleAt(ObstacleSystem *os, float x, float y)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!os->entities[i].active)
            continue;

        Entity *ent = &os->entities[i];

        if (x >= ent->x && x <= ent->x + obstacleWidth[i] &&
            y >= ent->y && y <= ent->y + obstacleHeight[i])
        {
            return ent;
        }
    }

    return NULL;
}

// ===== FUNÇÕES DE COLISÃO =====

int CheckBulletObstacleCollision(ObstacleSystem *obsSystem, BulletSystem *bulletSystem, float scrollY)
{
    if (!obsSystem || !bulletSystem)
        return 0;

    int totalPoints = 0;

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!obsSystem->entities[i].active)
            continue;

        Entity *ent = &obsSystem->entities[i];
        ObstacleType type = obstacleTypes[i];

        if (type <= OBSTACLE_TYPE_NONE || type > OBSTACLE_TYPE_PONTE)
            continue;

        // Converte obstáculo para coordenadas de tela
        float obsScreenX, obsScreenY;
        WorldToScreen(ent->x, ent->y, scrollY, &obsScreenX, &obsScreenY);

        Rectangle obsRect = {
            obsScreenX,
            obsScreenY,
            (float)obstacleWidth[i],
            (float)obstacleHeight[i]};

        // Verifica colisão com cada tiro
        for (int j = 0; j < MAX_BULLETS; j++)
        {
            if (!bulletSystem->bullets[j].active)
                continue;

            Bullet *bullet = &bulletSystem->bullets[j];

            // Tiro já está em coordenadas de tela
            Rectangle bulletRect = {
                bullet->x,
                bullet->y,
                (float)bullet->width,
                (float)bullet->height};

            if (CheckCollisionRecs(bulletRect, obsRect))
            {
                // Atingiu o obstáculo!
                obstacleHealth[i] -= bullet->damage;
                bullet->active = 0; // Destroi o tiro

                printf("ACERTOU %s! Saúde: %d/%d\n",
                       obstacleNames[type], obstacleHealth[i], obstacleTypeHealth[type]);

                // Se a saúde chegou a zero, destrói o obstáculo
                if (obstacleHealth[i] <= 0)
                {
                    obsSystem->entities[i].active = 0;
                    obsSystem->activeEntitiesCount--;
                    totalPoints += obstacleScore[i];

                    printf("%s DESTRUÍDO! +%d pontos\n",
                           obstacleNames[type], obstacleScore[i]);
                }

                break; // Este tiro já atingiu algo
            }
        }
    }

    return totalPoints;
}

void HandlePlayerObstacleCollision(ObstacleSystem *system, Player *player, float scrollY)
{
    if (!player || !player->active)
        return;

    // Pega a hitbox do jogador (em coordenadas de tela)
    Rectangle playerRect = GetPlayerHitbox(player);

    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!system->entities[i].active)
            continue;

        Entity *ent = &system->entities[i];
        ObstacleType type = obstacleTypes[i];

        if (type <= OBSTACLE_TYPE_NONE)
            continue;

        // Converte obstáculo para coordenadas de tela
        float obsScreenX, obsScreenY;
        WorldToScreen(ent->x, ent->y, scrollY, &obsScreenX, &obsScreenY);

        Rectangle obsRect = {
            obsScreenX,
            obsScreenY,
            (float)obstacleWidth[i],
            (float)obstacleHeight[i]};

        if (CheckCollisionRecs(playerRect, obsRect))
        {
            printf("COLISÃO detectada com %s\n", obstacleNames[type]);

            switch (type)
            {
            case OBSTACLE_TYPE_TERRA:
            case OBSTACLE_TYPE_NAVIO:
            case OBSTACLE_TYPE_HELICOPTERO:
            case OBSTACLE_TYPE_PONTE:
                // Mata o jogador (ou tira vida)
                player->active = 0;
                printf("COLISÃO FATAL com %s! Jogador morreu.\n",
                       obstacleNames[type]);
                break;

            case OBSTACLE_TYPE_GAS:
                // Reabastece
                Refuel(player, 30.0f);
                printf("REABASTECENDO no posto! Combustível: %.0f/100\n", player->fuel);
                break;

            default:
                break;
            }

            return;
        }
    }
}

// ===== FUNÇÕES ADICIONAIS ÚTEIS =====

void LoadObstaclesFromMap(ObstacleSystem *system, const Map *map)
{
    ClearObstacles(system);

    printf("Carregando obstáculos do mapa...\n");
    int count = 0;
    int terraCount = 0;

    // Array para marcar células já processadas
    int processed[MAP_ROWS][MAP_COLS] = {0};

    for (int y = 0; y < map->height; y++)
    {
        for (int x = 0; x < MAP_COLS; x++)
        {
            if (processed[y][x])
                continue;

            char cell = map->data[y][x];
            ObstacleType type = CharToObstacleType(cell);

            if (type != OBSTACLE_TYPE_NONE)
            {
                float worldX = x * TILE_SIZE;
                float worldY = y * TILE_SIZE;

                // Verifica se é um obstáculo multi-tile (Ponte ou Gasolina)
                if (type == OBSTACLE_TYPE_PONTE || type == OBSTACLE_TYPE_GAS)
                {
                    // Conta quantas células consecutivas do mesmo tipo existem
                    int consecutive = 1;
                    for (int dx = 1; x + dx < MAP_COLS; dx++)
                    {
                        if (map->data[y][x + dx] == cell)
                            consecutive++;
                        else
                            break;
                    }

                    // Se houver menos de 4, ajusta o tamanho
                    int widthInTiles = (consecutive >= 4) ? 4 : consecutive;

                    // Marca todas as células como processadas
                    for (int dx = 0; dx < consecutive; dx++)
                    {
                        if (x + dx < MAP_COLS)
                            processed[y][x + dx] = 1;
                    }

                    // Pula as células já processadas
                    x += (consecutive - 1);

                    printf("Obstáculo %s de %d tiles em (%.0f, %.0f)\n",
                           obstacleNames[type], widthInTiles, worldX, worldY);
                }
                else
                {
                    // Obstáculo de 1 tile
                    processed[y][x] = 1;

                    if (type == OBSTACLE_TYPE_TERRA)
                    {
                        terraCount++;
                    }
                }

                AddObstacle(system, worldX, worldY, type);
                count++;
            }
        }
    }

    printf("%d obstáculos carregados do mapa (%d blocos de terra)\n", count, terraCount);
}

int GetActiveObstacleCount(const ObstacleSystem *system)
{
    return system->activeEntitiesCount;
}

ObstacleType GetObstacleType(const Entity *entity)
{
    if (!entity)
        return OBSTACLE_TYPE_NONE;
    return CharToObstacleType(entity->type);
}

const char *GetObstacleTypeName(ObstacleType type)
{
    if (type >= OBSTACLE_TYPE_NONE && type <= OBSTACLE_TYPE_PONTE)
        return obstacleNames[type];
    return "UNKNOWN";
}

int GetObstacleScoreValue(ObstacleType type)
{
    if (type >= OBSTACLE_TYPE_NONE && type <= OBSTACLE_TYPE_PONTE)
        return obstacleTypeScores[type];
    return OBSTACLE_TYPE_NONE;
}