CC = gcc
CFLAGS = -Wall -std=c99 -O2 -g
RAYLIB_PATH = C:/raylib
INCLUDES = -I./src/include -I$(RAYLIB_PATH)/include
LIBS = -L$(RAYLIB_PATH)/lib -lraylib -lopengl32 -lgdi32 -lwinmm -luser32
SRC_DIR = src
CORE_DIR = $(SRC_DIR)/core
TESTS_DIR = $(SRC_DIR)/tests

# ALVO PRINCIPAL
all: river-inf.exe

# JOGO COMPLETO
river-inf: $(SRC_DIR)/main.c \
               $(CORE_DIR)/game.c \
               $(CORE_DIR)/player.c \
               $(CORE_DIR)/bullet.c \
               $(CORE_DIR)/obstacle.c \
               $(CORE_DIR)/map.c \
               $(CORE_DIR)/highscore.c
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LIBS)
	@echo Jogo compilado: river-inf.exe
	./river-inf.exe

# TESTE DE HIGHSCORE
test-highscore:$(TESTS_DIR)/test_highscore.c $(CORE_DIR)/highscore.c
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LIBS)
	@echo Teste de Highscore compilado!
	@echo Executando teste do módulo Highscore...
	./test-highscore.exe

# TESTE DE OBSTÁCULO
test-obstacle: $(TESTS_DIR)/test_obstacle.c $(CORE_DIR)/obstacle.c $(CORE_DIR)/bullet.c $(CORE_DIR)/player.c $(CORE_DIR)/map.c
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LIBS)
	@echo Teste de Obstáculos compilado!
	@echo Executando teste do sistema de obstáculos...
	./test-obstacle.exe	

# TESTE DE TIRO
test-bullet: $(TESTS_DIR)/test_bullet.c $(CORE_DIR)/bullet.c $(CORE_DIR)/player.c $(CORE_DIR)/map.c
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LIBS)
	@echo Teste de Tiros compilado!
	@echo Executando teste do sistema de tiros...
	./test-bullet.exe

# TESTE DO PLAYER
test-player: $(TESTS_DIR)/test_player.c $(CORE_DIR)/player.c $(CORE_DIR)/map.c
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LIBS)
	@echo Teste do Player compilado!
	@echo Executando teste do módulo Player...
	./test-player.exe

# TESTE DO MAPA
test-map: $(TESTS_DIR)/test_map.c $(CORE_DIR)/map.c
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LIBS)
	@echo Teste do mapa compilado
	@echo Executando teste do mapa...
	./test-map.exe

# COMPILAR E EXECUTAR JOGO
run: river-inf.exe
	@echo Executando jogo...
	./river-inf.exe

# LIMPAR
clean:
	@del /Q *.exe *.o 2>nul || true
	@echo Arquivos limpos!

# AJUDA
help:
	@echo.
	@echo COMANDOS DISPONÍVEIS:
	@echo   make all       - Compila o jogo completo
	@echo   make run       - Compila e executa o jogo
	@echo   make test-map  - Compila e testa o módulo mapa
	@echo   make clean     - Limpa arquivos compilados
	@echo   make help      - Mostra esta ajuda
	@echo.

# ALIAS (atalhos)
test: test-map
build: all

.PHONY: all clean run test-map help test build