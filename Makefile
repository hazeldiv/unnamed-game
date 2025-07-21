CC = gcc
CFLAGS = -I src/include
LDFLAGS = -Lsrc/lib/SDL3 -lSDL3 -Lsrc/lib/enet -lenet -lm -lws2_32 -lwinmm

SRC = src/server.c
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