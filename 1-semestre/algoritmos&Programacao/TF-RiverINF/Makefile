CC = gcc
SRC = src/main.c $(wildcard src/core/*.c)
INCLUDES = -I src/includes
LIBS = -L C:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm
OUT = river-inf.exe

all:
	$(CC) $(SRC) $(INCLUDES) $(LIBS) -o $(OUT)

clean:
	del $(OUT)
