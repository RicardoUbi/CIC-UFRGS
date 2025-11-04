#include "background.h"
#include "game.h"

void DrawBackground(float offset)
{
    DrawRectangle(0, 0, 80, SCREEN_HEIGHT, GREEN);
    DrawRectangle(SCREEN_WIDTH - 80, 0, 80, SCREEN_HEIGHT, GREEN);

    DrawRectangle(80, (int)offset, SCREEN_WIDTH - 160, SCREEN_HEIGHT, (Color){0, 102, 204, 255});
    DrawRectangle(80, (int)offset - SCREEN_HEIGHT, SCREEN_WIDTH - 160, SCREEN_HEIGHT, (Color){0, 102, 204, 255});
}
