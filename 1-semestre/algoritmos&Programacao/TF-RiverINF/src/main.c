#include "raylib.h"
#include "include/game.h"

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "RIVER-INF - Ricardo 598925");
    InitAudioDevice();
    SetTargetFPS(60);

    Game game = InitGame();

    while (!WindowShouldClose())
    {
        UpdateGame(&game);
        DrawGame(&game);
    }

    UnloadGame(&game);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
