CC = gcc
CFLAGS = -I src/include
LDFLAGS = -Lsrc/lib/SDL3 -lSDL3 -Lsrc/lib/enet -lenet -lm -lws2_32 -lwinmm

MAIN_SRC = src/mainServer.c
WORLD_SRC = src/server.c

MAIN_OBJ = $(patsubst src/%.c, build/%.o, $(MAIN_SRC))
WORLD_OBJ = $(patsubst src/%.c, build/%.o, $(WORLD_SRC))

MAIN_BIN = main.exe
WORLD_BIN = server.exe

all: $(MAIN_BIN) $(WORLD_BIN)

$(MAIN_BIN): $(MAIN_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(WORLD_BIN): $(WORLD_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f build/*.o *.exe
	rm -rf build

run: all
	./main