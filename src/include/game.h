#ifndef GAME_H
#define GAME_H
#include <entity.h>
#include <SDL3/SDL.h>
#include <block.h>
#include <input.h>
#include <inventory.h>
#include <tile.h>
#include <scene.h>
#include <SDL3_ttf/SDL_ttf.h>

#define WORLD_WIDTH 100
#define WORLD_HEIGHT 100

typedef struct game {
    SDL_Window* window;
    SDL_Renderer* renderer;
    tile world[WORLD_HEIGHT][WORLD_WIDTH];
    entity entityState;
    float deltaTime;
    int screenWidth;
    int screenHeight;
    tile displayedTile[30][30];
    input input;
    inventory inventory;
    entity surroundingPlayer[32];
    int isClientBusy;
    float t;
    Uint64 serverOffsetTime;
    int currentScene;
    scene scene[10];
    TTF_TextEngine *engine;
    TTF_Font *font;
} game;

#endif