CC = gcc
CFLAGS = -I src/include
LDFLAGS = -L src/lib/SDL3 -lSDL3 -L src/lib/SDL3_image -lSDL3_image -lm

SRC = src/main.c src/renderer.c src/player.c src/physics.c src/vec2.c src/input.c src/inventory.c src/item.c src/tile.c
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