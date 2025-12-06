#ifndef PLAYER_H
#define PLAYER_H

#include "defines.h"

typedef struct {
    float x, y; // Posição do jogador
    float fuel; // Combustível do jogador
    int active; // Se o jogador estiver ativo (Vivo)
    int score;  // Pontuação do jogador
} Player;

void InitPlayer(Player *player);
void UpdatePlayer(Player *player);
void DrawPlayer(const Player *player);
void MovePlayer(Player *player);
void ConsumeFuel(Player *player, float amount);
void Refuel(Player *player, float amount);
int CheckPlayerCollision(const Player *player, float objX, float objY, float objWidth, float objHeight);

#endif
