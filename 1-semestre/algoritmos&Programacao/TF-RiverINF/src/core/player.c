#include "player.h"
#include "raylib.h"
#include <stdio.h>

#define PLAYER_WIDTH 30
#define PLAYER_HEIGHT 30
#define PLAYER_SPEED 5.0f
#define START_FUEL 100.0f
#define FUEL_CONSUMPTION 0.025f
#define INVINCIBILITY_TIME 2.0f

static const char *skinPaths[MAX_SKINS] = {
    "src/assets/airplanes/air1.png",
    "src/assets/airplanes/air2.png",
    "src/assets/airplanes/air3.png",
    "src/assets/airplanes/air4.png",
    "src/assets/airplanes/air5.png"};

static Texture2D playerTextures[MAX_SKINS] = {0};
static int texturesLoaded = 0;

void LoadPlayerTextures(Player *player)
{
    if (texturesLoaded)
        return;

    printf("Carregando texturas do jogador...\n");

    for (int i = 0; i < MAX_SKINS; i++)
    {
        if (FileExists(skinPaths[i]))
        {
            playerTextures[i] = LoadTexture(skinPaths[i]);
            printf("Skin %d: %s (%dx%d)\n",
                   i, skinPaths[i],
                   playerTextures[i].width,
                   playerTextures[i].height);
        }
        else
        {
            printf("Skin %d não encontrada: %s\n", i, skinPaths[i]);
            // Carrega uma textura padrão (retângulo colorido)
            Image defaultImg = GenImageColor(64, 64,
                                             (i == 0) ? YELLOW : (i == 1) ? RED
                                                             : (i == 2)   ? BLUE
                                                             : (i == 3)   ? GREEN
                                                                          : PURPLE);
            playerTextures[i] = LoadTextureFromImage(defaultImg);
            UnloadImage(defaultImg);
        }
    }

    texturesLoaded = 1;
    printf("Todas as texturas carregadas!\n");
}

void UnloadPlayerTextures(Player *player)
{
    if (!texturesLoaded)
        return;

    for (int i = 0; i < MAX_SKINS; i++)
    {
        UnloadTexture(playerTextures[i]);
    }

    texturesLoaded = 0;
    printf("Texturas do jogador descarregadas\n");
}

void InitPlayer(Player *player)
{
    LoadPlayerTextures(player);

    // Posiciona o jogador no centro da tela
    player->x = SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2;
    player->y = SCREEN_HEIGHT - 90;
    player->fuel = START_FUEL;
    player->active = 1;
    player->score = 0;
    player->invincible = 0;
    player->invincibleTimer = 0;

    // Skin padrão
    player->currentSkin = 0;
    player->texture = playerTextures[player->currentSkin];
    player->scale = PLAYER_SCALE;

    // Calcula hitbox
    player->hitbox.width = player->texture.width * player->scale * HITBOX_SCALE;
    player->hitbox.height = player->texture.height * player->scale * HITBOX_SCALE;
    player->hitbox.x = player->x - player->hitbox.width / 2;
    player->hitbox.y = player->y - player->hitbox.height / 2;

    printf("Jogador inicializado\n");
    printf("Skin: %d (%s)\n", player->currentSkin, skinPaths[player->currentSkin]);
    printf("Posição: (%.0f, %.0f)\n", player->x, player->y);
    printf("Hitbox: %.0fx%.0f\n", player->hitbox.width, player->hitbox.height);
}

void UpdatePlayer(Player *player)
{
    if (!player->active)
        return;

    MovePlayer(player);

    ConsumeFuel(player, FUEL_CONSUMPTION);

    if (player->invincible)
    {
        player->invincibleTimer -= GetFrameTime();
        if (player->invincibleTimer <= 0)
        {
            player->invincible = 0;
            player->invincibleTimer = 0;
        }
    }

    if (player->fuel <= 0)
    {
        player->fuel = 0;
        player->active = 0;
        printf("Jogador morreu sem combustivel!\n");
    }

    player->hitbox.x = player->x - player->hitbox.width / 2;
    player->hitbox.y = player->y - player->hitbox.height / 2;
}

void DrawPlayer(const Player *player)
{
    if (!player->active)
        return;

    // Calcula dimensões escaladas
    float width = player->texture.width * player->scale;
    float height = player->texture.height * player->scale;

    // Posição para centralizar
    float drawX = player->x - width / 2;
    float drawY = player->y - height / 2;

    DrawTextureEx(player->texture,
                  (Vector2){drawX, drawY},
                  0.0f,
                  player->scale,
                  WHITE);

    // Efeito de piscar se invencível
    if (player->invincible)
    {
        // Pisca a cada 0.2 segundos
        if (((int)(GetTime() * 10) % 2) == 0)
        {
            DrawRectangleLines(drawX, drawY, width, height, YELLOW);
        }
    }
}

void MovePlayer(Player *player)
{
    if (!player->active)
        return;

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
    {
        player->x -= PLAYER_SPEED;
    }

    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
    {
        player->x += PLAYER_SPEED;
    }

    float halfWidth = (player->texture.width * player->scale) / 2;

    if (player->x < halfWidth)
    {
        player->x = halfWidth;
    }

    if (player->x > SCREEN_WIDTH - halfWidth)
    {
        player->x = SCREEN_WIDTH - halfWidth;
    }
}

void ConsumeFuel(Player *player, float amount)
{
    player->fuel -= amount;

    if (player->fuel < 0)
    {
        player->fuel = 0;
    }
}

void Refuel(Player *player, float amount)
{
    player->fuel += amount;

    if (player->fuel > START_FUEL)
    {
        player->fuel = START_FUEL;
    }
}

int CheckPlayerCollision(const Player *player, float objX, float objY, float objWidth, float objHeight)
{
    if (!player->active || player->invincible)
        return 0;

    Rectangle objRect = {objX, objY, objWidth, objHeight};
    return CheckCollisionRecs(player->hitbox, objRect);
}

void ChangePlayerSkin(Player *player, int skinIndex)
{
    if (skinIndex < 0 || skinIndex >= MAX_SKINS)
    {
        printf("Índice de skin inválido: %d\n", skinIndex);
        return;
    }

    player->currentSkin = skinIndex;
    player->texture = playerTextures[skinIndex];

    // Recalcula hitbox para nova textura
    player->hitbox.width = player->texture.width * player->scale * HITBOX_SCALE;
    player->hitbox.height = player->texture.height * player->scale * HITBOX_SCALE;

    printf("Skin alterada para: %d\n", skinIndex);
}

int GetPlayerSkinCount(void)
{
    return MAX_SKINS;
}

void NextPlayerSkin(Player *player)
{
    int newSkin = (player->currentSkin + 1) % MAX_SKINS;
    ChangePlayerSkin(player, newSkin);
}

void PreviousPlayerSkin(Player *player)
{
    int newSkin = (player->currentSkin - 1 + MAX_SKINS) % MAX_SKINS;
    ChangePlayerSkin(player, newSkin);
}

void PlayerTakeDamage(Player *player)
{
    if (player->invincible)
        return;

    player->lives--;
    player->invincible = 1;
    player->invincibleTimer = INVINCIBILITY_TIME;

    printf("Dano! Vidas restantes: %d\n", player->lives);

    if (player->lives <= 0)
    {
        player->active = 0;
        printf("Jogador derrotado!\n");
    }
}

int PlayerIsInvincible(const Player *player)
{
    return player->invincible;
}