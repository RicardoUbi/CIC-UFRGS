#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../include/highscore.h"

#define TEST_WINDOW_WIDTH 800
#define TEST_WINDOW_HEIGHT 600

// Função para testar todas as funcionalidades
void RunAllTests(HighScoreEntry *highScores)
{
    printf("\n=== EXECUTANDO TESTES DE HIGHSCORE ===\n");
    
    // Teste 1: Carregar highscores
    printf("\n1. Carregando highscores...\n");
    LoadHighScores(highScores);
    
    // Teste 2: Verificar se é highscore
    printf("\n2. Verificando pontuações...\n");
    int testScores[] = {500, 1500, 800, 950, 1200};
    for (int i = 0; i < 5; i++)
    {
        int isHigh = IsHighScore(highScores, testScores[i]);
        printf("Pontuação %d: %s um highscore\n", 
               testScores[i], 
               isHigh ? "É" : "NÃO É");
    }
    
    // Teste 3: Adicionar novos highscores
    printf("\n3. Adicionando novos highscores...\n");
    
    // Limpa highscores existentes para teste
    ResetHighScores(highScores);
    SaveHighScores(highScores);
    
    // Adiciona alguns highscores de teste
    AddHighScore(highScores, 2500, "CHAMPION");
    AddHighScore(highScores, 1800, "ACE PILOT");
    AddHighScore(highScores, 1200, "ROOKIE");
    AddHighScore(highScores, 900, "BEGINNER");
    AddHighScore(highScores, 600, "TRAINEE");
    
    // Teste 4: Ordenação
    printf("\n4. Ordenando highscores...\n");
    SortHighScores(highScores);
    
    // Teste 5: Verificar posição
    printf("\n5. Verificando posições...\n");
    int newScores[] = {2000, 1500, 800, 300};
    for (int i = 0; i < 4; i++)
    {
        int pos = GetHighScorePosition(highScores, newScores[i]);
        if (pos != -1)
        {
            printf("   Pontuação %d entraria na posição %d\n", newScores[i], pos + 1);
        }
        else
        {
            printf("   Pontuação %d não é um highscore\n", newScores[i]);
        }
    }
    
    printf("\n=== TESTES CONCLUÍDOS ===\n");
}

// Função para desenhar a tela de teste
void DrawTestScreen(const HighScoreEntry *highScores, int testPhase, const char *testMessage)
{
    BeginDrawing();
    {
        ClearBackground(BLACK);
        
        // Título
        DrawText("TESTE DE HIGHSCORE SYSTEM", 100, 50, 30, YELLOW);
        
        // Highscores atuais
        DrawHighScores(highScores, 100, 120, 20, WHITE);
        
        // Informações do teste
        DrawRectangle(100, 400, 600, 150, (Color){0, 0, 0, 200});
        
        DrawText("COMANDOS DE TESTE:", 120, 420, 20, GREEN);
        DrawText("R - Reset highscores", 140, 450, 18, WHITE);
        DrawText("L - Carregar highscores", 140, 480, 18, WHITE);
        DrawText("S - Salvar highscores", 140, 510, 18, WHITE);
        DrawText("T - Executar todos os testes", 140, 540, 18, WHITE);
        DrawText("ESC - Sair", 140, 570, 18, WHITE);
        
        // Mensagem do teste atual
        if (testMessage)
        {
            DrawText(testMessage, 120, 370, 18, ORANGE);
        }
        
        // Status do arquivo
        if (IsFileExtension(HIGHSCORE_FILE, ".bin"))
        {
            DrawText(TextFormat("Arquivo: %s", HIGHSCORE_FILE), 500, 50, 16, GRAY);
        }
    }
    EndDrawing();
}

int main(void)
{
    // Inicialização
    InitWindow(TEST_WINDOW_WIDTH, TEST_WINDOW_HEIGHT, "Teste: Sistema de Highscore");
    SetTargetFPS(60);
    
    printf("================================\n");
    printf("TESTE DO SISTEMA DE HIGHSCORE\n");
    printf("================================\n");
    printf("Este teste verifica todas as funcionalidades do sistema de highscores.\n");
    printf("Highscores são salvos em: %s\n", HIGHSCORE_FILE);
    printf("\nComandos disponíveis:\n");
    printf("  R - Reset highscores\n");
    printf("  L - Carregar highscores\n");
    printf("  S - Salvar highscores\n");
    printf("  T - Executar todos os testes\n");
    printf("  ESC - Sair\n");
    printf("================================\n");
    
    // Array de highscores
    HighScoreEntry highScores[MAX_HIGHSCORES];
    
    // Carrega highscores iniciais
    LoadHighScores(highScores);
    
    // Variáveis de controle
    int testPhase = 0;
    char testMessage[100] = {0};
    clock_t lastUpdate = clock();
    
    // Loop principal
    while (!WindowShouldClose())
    {
        
        // Controles de teste
        if (IsKeyPressed(KEY_R))
        {
            ResetHighScores(highScores);
            strcpy(testMessage, "Highscores resetados!");
            lastUpdate = clock();
        }
        
        if (IsKeyPressed(KEY_L))
        {
            LoadHighScores(highScores);
            strcpy(testMessage, "Highscores carregados!");
            lastUpdate = clock();
        }
        
        if (IsKeyPressed(KEY_S))
        {
            SaveHighScores(highScores);
            strcpy(testMessage, "Highscores salvos!");
            lastUpdate = clock();
        }
        
        if (IsKeyPressed(KEY_T))
        {
            RunAllTests(highScores);
            strcpy(testMessage, "Todos os testes executados! Verifique o console.");
            lastUpdate = clock();
        }
        
        // Teste interativo: Adicionar pontuação aleatória
        if (IsKeyPressed(KEY_SPACE))
        {
            int randomScore = GetRandomValue(500, 2500);
            char randomName[20];
            
            // Gera um nome aleatório
            const char *names[] = {"ALEX", "BIA", "CARLOS", "DANI", "EDU", "FABIO", "GABI", "HENRIQUE"};
            strcpy(randomName, names[GetRandomValue(0, 7)]);
            
            if (IsHighScore(highScores, randomScore))
            {
                AddHighScore(highScores, randomScore, randomName);
                sprintf(testMessage, "Adicionado: %s - %d pontos", randomName, randomScore);
            }
            else
            {
                sprintf(testMessage, "%d pontos não é um highscore (mínimo: %d)", 
                       randomScore, highScores[MAX_HIGHSCORES - 1].score);
            }
            lastUpdate = clock();
        }
        
        // Limpa mensagem após 3 segundos
        if ((clock() - lastUpdate) / CLOCKS_PER_SEC > 3)
        {
            testMessage[0] = '\0';
        }
        
        // Renderização
        DrawTestScreen(highScores, testPhase, testMessage);
    }
    
    // Salva antes de sair
    SaveHighScores(highScores);
    
    // Limpeza
    CloseWindow();
    
    printf("\n================================\n");
    printf("Teste finalizado\n");
    printf("Highscores finais salvos em: %s\n", HIGHSCORE_FILE);
    printf("Último highscore: %s - %d pontos\n", 
           highScores[0].name, highScores[0].score);
    printf("================================\n");
    
    return 0;
}