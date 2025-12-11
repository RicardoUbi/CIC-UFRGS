#include "highscore.h"
#include "game.h"
#include <stdio.h>
#include <string.h>

static int IsValidEntry(const HighScoreEntry *e)
{
    if (!e) return 0;

    if (e->score < 0 || e->score > 100000000)
        return 0;

    if (strlen(e->name) == 0 || strlen(e->name) >= MAX_NAME_LENGTH)
        return 0;

    return 1;
}

void ResetHighScores(HighScoreEntry *highScores)
{
    if (!highScores) return;

    for (int i = 0; i < MAX_HIGHSCORES; i++)
    {
        strcpy(highScores[i].name, "----------");
        highScores[i].score = 0;
    }

    //printf("Highscores resetados (zerados)\n");
}

void LoadHighScores(HighScoreEntry *highScores)
{
    if (!highScores) return;

    FILE *file = fopen(HIGHSCORE_FILE, "rb");
    if (!file)
    {
        //printf("Arquivo de highscore não existe, criando novo\n");
        ResetHighScores(highScores);
        SaveHighScores(highScores);
        return;
    }

    size_t read = fread(highScores, sizeof(HighScoreEntry), MAX_HIGHSCORES, file);
    fclose(file);

    if (read != MAX_HIGHSCORES)
    {
        //printf("Arquivo incompleto, resetando highscores\n");
        ResetHighScores(highScores);
        SaveHighScores(highScores);
        return;
    }

    // Validação defensiva
    for (int i = 0; i < MAX_HIGHSCORES; i++)
    {
        if (!IsValidEntry(&highScores[i]))
        {
            //printf("Entrada inválida detectada, resetando highscores\n");
            ResetHighScores(highScores);
            SaveHighScores(highScores);
            return;
        }
    }

    //printf("Highscores carregados com sucesso\n");
}

void SaveHighScores(const HighScoreEntry *highScores)
{
    if (!highScores) return;

    FILE *file = fopen(HIGHSCORE_FILE, "wb");
    if (!file)
    {
        //printf("Erro ao salvar highscores\n");
        return;
    }

    fwrite(highScores, sizeof(HighScoreEntry), MAX_HIGHSCORES, file);
    fclose(file);
}

int IsHighScore(const HighScoreEntry *highScores, int newScore)
{
    if (!highScores) return 0;
    return newScore > highScores[MAX_HIGHSCORES - 1].score;
}

int GetHighScorePosition(const HighScoreEntry *highScores, int newScore)
{
    if (!highScores) return -1;

    for (int i = 0; i < MAX_HIGHSCORES; i++)
        if (newScore > highScores[i].score)
            return i;

    return -1;
}

void AddHighScore(HighScoreEntry *highScores, int newScore, const char *name)
{
    if (!highScores || !name) return;

    int pos = GetHighScorePosition(highScores, newScore);
    if (pos < 0) return;

    for (int i = MAX_HIGHSCORES - 1; i > pos; i--)
        highScores[i] = highScores[i - 1];

    highScores[pos].score = newScore;
    strncpy(highScores[pos].name, name, MAX_NAME_LENGTH - 1);
    highScores[pos].name[MAX_NAME_LENGTH - 1] = '\0';

    SaveHighScores(highScores);

    //printf("Highscore salvo: %s - %d (posição %d)\n",
           highScores[pos].name, newScore, pos + 1);
}
