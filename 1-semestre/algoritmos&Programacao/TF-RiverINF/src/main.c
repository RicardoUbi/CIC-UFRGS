#include "raylib.h"
#include "game.h"
#include <stdio.h>

int main(void)
{
    // Configuração da janela
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "River Raid INF - CIC/UFRGS");

    if (!IsWindowReady())
    {
        //printf("Erro ao criar a janela!\n");
        return 1;
    }

    SetTargetFPS(60);
    SetExitKey(0); // Desabilita ESC para sair direto

    // Inicializa o jogo
    Game game;
    InitGame(&game);

    // Loop principal do jogo
    while (!WindowShouldClose())
    {
        // Atualiza tamanho da janela se redimensionada
        if (IsWindowResized())
        {
            int width = GetScreenWidth();
            int height = GetScreenHeight();

            // Mínimo de tamanho
            if (width < 400)
                width = 400;
            if (height < 300)
                height = 300;

            SetWindowSize(width, height);
            UpdateWindowSize(&game, width, height);
        }

        // Atualiza o jogo
        UpdateGame(&game);

        // Renderiza
        BeginDrawing();
        {
            ClearBackground(BLACK);
            DrawGame(&game);
        }
        EndDrawing();
    }

    // Limpeza
    UnloadGameResources(&game);
    CloseWindow();

    //printf("\n================================\n");
    //printf("Jogo finalizado\n");
    //printf("Pontuação final: %d\n", game.score);
    //printf("Melhor highscore: %s - %d\n",
           game.highScores[0].name, game.highScores[0].score);
    //printf("================================\n");

    return 0;
}