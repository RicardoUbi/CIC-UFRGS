#include "bullet.h"
#include "raylib.h"
#include <stdio.h>

#define BULLET_WIDTH 4
#define BULLET_HEIGHT 10
#define BULLET_SPEED 10.0f
#define BULLET_COLOR WHITE
#define FIRE_RATE 0.2f

void InitBulletSystem(BulletSystem *system)
{
    // Inicializa todos os tiros como inativos
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        system->bullets[i].active = 0;
        system->bullets[i].x = 0;
        system->bullets[i].y = 0;
        system->bullets[i].speed = BULLET_SPEED;
        system->bullets[i].color = BULLET_COLOR;
        system->bullets[i].width = BULLET_WIDTH;
        system->bullets[i].height = BULLET_HEIGHT;
        system->bullets[i].damage = 1;
    }

    system->activeCount = 0;
    system->fireRate = FIRE_RATE;
    system->lastShotTime = 0;
    system->shootSound = (Sound){0};

    // Tenta carregar som (opcional)
    if (IsAudioDeviceReady())
    {
        if (FileExists("src/assets/sounds/shoot.wav"))
        {
            system->shootSound = LoadSound("src/assets/sounds/shoot.wav");
            printf("Som de tiro carregado\n");
        }
        else
        {
            printf("Som de tiro não encontrado\n");
        }
    }

    printf("Sistema de tiros inicializado\n");
    printf("Capacidade: %d tiros\n", MAX_BULLETS);
    printf("Fire rate: %.1f tiros/segundo\n", 1.0f / FIRE_RATE);
}

void UnloadBulletSystem(BulletSystem *system)
{
    if (system->shootSound.frameCount > 0)
    {
        UnloadSound(system->shootSound);
        printf("Som de tiro descarregado\n");
    }
    printf("Sistema de tiros descarregado\n");
}

void FireBullet(BulletSystem *system, float startX, float startY)
{
    // Verifica rate limit
    float currentTime = GetTime();
    if (currentTime - system->lastShotTime < system->fireRate)
    {
        return; // Ainda não pode atirar
    }

    // Encontra um tiro inativo
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (!system->bullets[i].active)
        {
            // Ativa o tiro
            system->bullets[i].active = 1;
            system->bullets[i].x = startX - (BULLET_WIDTH / 2); // Centraliza
            system->bullets[i].y = startY;
            system->bullets[i].speed = BULLET_SPEED;

            system->activeCount++;
            system->lastShotTime = currentTime;

            if (system->shootSound.frameCount > 0)
            {
                PlaySound(system->shootSound);
            }

            // DEBUG
            // printf("Tiro disparado #%d em (%.0f, %.0f)\n", i, startX, startY);
            return;
        }
    }

    // Se chegou aqui, não há tiros disponíveis
    printf("Nenhum tiro disponível! (MAX: %d)\n", MAX_BULLETS);
}

void FireBulletFromPlayer(BulletSystem *system, const Player *player)
{
    if (!player->active)
        return;

    // Calcula posição do tiro (centro do jogador, um pouco acima)
    float bulletX = player->x;
    float bulletY = player->y - 10; // Um pouco acima do jogador

    FireBullet(system, bulletX, bulletY);
}

void UpdateBullets(BulletSystem *system)
{
    system->activeCount = 0;

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (system->bullets[i].active)
        {
            // Move o tiro para cima
            system->bullets[i].y -= system->bullets[i].speed;

            // Desativa se sair da tela
            if (system->bullets[i].y + BULLET_HEIGHT < 0)
            {
                system->bullets[i].active = 0;
            }
            else
            {
                system->activeCount++;
            }
        }
    }
}

void ClearBullets(BulletSystem *system)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        system->bullets[i].active = 0;
    }
    system->activeCount = 0;
    printf("Todos os tiros foram limpos\n");
}

void DrawBullets(const BulletSystem *system)
{
    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (system->bullets[i].active)
        {
            DrawRectangle(system->bullets[i].x,
                          system->bullets[i].y,
                          system->bullets[i].width,
                          system->bullets[i].height,
                          system->bullets[i].color);

            // Efeito visual opcional: brilho no tiro
            DrawRectangle(system->bullets[i].x + 1,
                          system->bullets[i].y + 1,
                          system->bullets[i].width - 2,
                          system->bullets[i].height - 2,
                          YELLOW);
        }
    }
}

int CheckBulletCollision(const BulletSystem *system, float objX, float objY, float objWidth, float objHeight)
{
    Rectangle objRect = {objX, objY, objWidth, objHeight};

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (system->bullets[i].active)
        {
            Rectangle bulletRect = {
                system->bullets[i].x,
                system->bullets[i].y,
                system->bullets[i].width,
                system->bullets[i].height};

            if (CheckCollisionRecs(bulletRect, objRect))
            {
                return 1; // Colidiu
            }
        }
    }

    return 0; // Não colidiu
}

Bullet *GetCollidingBullet(BulletSystem *system, float objX, float objY, float objWidth, float objHeight)
{
    Rectangle objRect = {objX, objY, objWidth, objHeight};

    for (int i = 0; i < MAX_BULLETS; i++)
    {
        if (system->bullets[i].active)
        {
            Rectangle bulletRect = {
                system->bullets[i].x,
                system->bullets[i].y,
                system->bullets[i].width,
                system->bullets[i].height};

            if (CheckCollisionRecs(bulletRect, objRect))
            {
                return &system->bullets[i]; // Retorna ponteiro para o tiro
            }
        }
    }

    return NULL; // Nenhum tiro colidiu
}

int GetActiveBulletCount(const BulletSystem *system)
{
    return system->activeCount;
}

int CanShoot(const BulletSystem *system)
{
    float currentTime = GetTime();
    return (currentTime - system->lastShotTime >= system->fireRate);
}