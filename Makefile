CC = gcc
CFLAGS = -I src/include
LDFLAGS = -Lsrc/lib/SDL3 -lSDL3 -Lsrc/lib/SDL3_image -lSDL3_image -Lsrc/lib/SDL3_ttf -lSDL3_ttf -Lsrc/lib/enet -lenet -lm -lws2_32 -lwinmm

SRC = src/main.c src/renderer.c src/player.c src/physics.c src/vec2.c src/input.c src/inventory.c src/item.c src/tile.c src/client.c src/component.c src/mainMenu.c src/auth.c
OBJ = $(patsubst src/%.c, build/%.o, $(SRC))

all: main

main: $(OBJ)
	$(CC) ${CFLAGS} -o $@ $^ $(LDFLAGS)

build/%.o: src/%.c
	@if not exist build mkdir build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@if exist build\*.o del /Q build\*.o
	@if exist main.exe del /Q main.exe
	@if exist build rmdir /S /Q build

run: all
	./main