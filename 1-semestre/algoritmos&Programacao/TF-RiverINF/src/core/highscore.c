#include "highscore.h"
#include <stdio.h>
#include <string.h>

void LoadHighscores(Highscore highscores[])
{
    FILE *file = fopen("highscore.bin", "rb");
    if (file)
    {
        fread(highscores, sizeof(Highscore), MAX_HIGHSCORES, file);
        fclose(file);
    }
    else
    {
        // Inicializa com valores padrão
        for (int i = 0; i < MAX_HIGHSCORES; i++)
        {
            sprintf(highscores[i].name, "Player");
            highscores[i].score = 1000 - (i * 100);
        }
    }
}

void SaveHighscores(Highscore highscores[])
{
    FILE *file = fopen("highscore.bin", "wb");
    if (file)
    {
        fwrite(highscores, sizeof(Highscore), MAX_HIGHSCORES, file);
        fclose(file);
    }
}

void AddHighscore(Highscore highscores[], int score, const char *name)
{
    int pos = -1;
    for (int i = 0; i < MAX_HIGHSCORES; i++)
    {
        if (score > highscores[i].score)
        {
            pos = i;
            break;
        }
    }

    if (pos != -1)
    {
        for (int i = MAX_HIGHSCORES - 1; i > pos; i--)
        {
            highscores[i] = highscores[i - 1];
        }
        strcpy(highscores[pos].name, name);
        highscores[pos].score = score;
    }
}

int CheckHighscore(Highscore highscores[], int score)
{
    return score > highscores[MAX_HIGHSCORES - 1].score;
}
