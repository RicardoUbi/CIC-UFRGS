#ifndef BULLET_H
#define BULLET_H

#include "defines.h"
#include "player.h"

typedef struct {
    float x, y;           // Posição do tiro
    int active;           // 1 = ativo, 0 = inativo
    float speed;          // Velocidade do tiro
    Color color;          // Cor do tiro
    int width, height;    // Dimensões
    int damage;           // Dano causado (padrão: 1)
} Bullet;

typedef struct {
    Bullet bullets[MAX_BULLETS];  // Array de tiros
    int activeCount;              // Quantos tiros ativos
    float fireRate;               // Delay entre tiros (em segundos)
    float lastShotTime;           // Último tempo de disparo
    Sound shootSound;             // Som do tiro (opcional)
} BulletSystem;

// Inicialização
void InitBulletSystem(BulletSystem* system);
void UnloadBulletSystem(BulletSystem* system);

// Controle de tiros
void FireBullet(BulletSystem* system, float startX, float startY);
void FireBulletFromPlayer(BulletSystem* system, const Player* player);
void UpdateBullets(BulletSystem* system);
void ClearBullets(BulletSystem* system);

// Renderização
void DrawBullets(const BulletSystem* system);

// Colisão
int CheckBulletCollision(const BulletSystem* system, float objX, float objY, float objWidth, float objHeight);
Bullet* GetCollidingBullet(BulletSystem* system, float objX, float objY, float objWidth, float objHeight);

// Consulta
int GetActiveBulletCount(const BulletSystem* system);
int CanShoot(const BulletSystem* system);

#endif