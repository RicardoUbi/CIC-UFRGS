#include "background.h"
#include "defines.h"
#include "game.h"

void DrawBackground(float offset)
{
    int margin = 5 * TILE_SIZE; // 5 tiles de margem
    
    // Margens (terra) - sempre visíveis
    DrawRectangle(0, 0, margin, SCREEN_HEIGHT, GREEN);
    DrawRectangle(SCREEN_WIDTH - margin, 0, margin, SCREEN_HEIGHT, GREEN);

    // Rio (com scrolling infinito)
    Color riverColor = (Color){0, 102, 204, 255};
    
    // Desenhar duas cópias do rio para criar scrolling contínuo
    int riverWidth = SCREEN_WIDTH - 2 * margin;
    int riverY1 = (int)offset;
    int riverY2 = riverY1 - SCREEN_HEIGHT;
    
    DrawRectangle(margin, riverY1, riverWidth, SCREEN_HEIGHT, riverColor);
    DrawRectangle(margin, riverY2, riverWidth, SCREEN_HEIGHT, riverColor);
}