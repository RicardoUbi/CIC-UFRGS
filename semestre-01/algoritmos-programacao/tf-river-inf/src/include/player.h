#ifndef PLAYER_H
#define PLAYER_H

#include "defines.h"
#include "raylib.h"

typedef struct {
    float x, y;             // Posição do jogador
    float fuel;             // Combustível do jogador
    int active;             // Se o jogador estiver ativo (Vivo)
    int score;              // Pontuação do jogador
    Texture2D texture;      // Textura do jogador
    Rectangle hitbox;       // Hitbox do jogador
    float scale;            // Escala do jogador
    int currentSkin;        // Skin atual
    int lives;              // Vidas do jogador
    int invincible;         // Se o jogador estiver invulnerável
    float invincibleTimer;  // Contador de tempo de invulnerabilidade
} Player;

// Inicialização
void InitPlayer(Player *player);
void LoadPlayerTextures(Player *player);
void UnloadPlayerTextures(Player *player);

// Atualização
void UpdatePlayer(Player *player);
void DrawPlayer(const Player *player);
void MovePlayer(Player *player);

// Combustível
void ConsumeFuel(Player *player, float amount);
void Refuel(Player *player, float amount);

// Colisão
int CheckPlayerCollision(const Player *player, float objX, float objY, float objWidth, float objHeight);
Rectangle GetPlayerHitbox(const Player *player); // implementar, não existe no contexto

// Sistema de skins
void ChangePlayerSkin(Player *player, int skinIndex);
int GetPlayerSkinCount(void);
void NextPlayerSkin(Player *player);
void PreviousPlayerSkin(Player *player);

// Sistema de vidas/dano
void PlayerTakeDamage(Player *player);
int PlayerIsInvincible(const Player *player);

#endif
