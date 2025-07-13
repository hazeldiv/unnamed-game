#ifndef GAME_H
#define GAME_H
#include <entity.h>
#include <SDL3/SDL.h>
#include <block.h>
#include <input.h>
#include <inventory.h>
#include <tile.h>

typedef struct game {
    SDL_Window* window;
    SDL_Renderer* renderer;
    //int world[100][100];
    //int background[100][100];
    tile world[100][100];
    entity entityState;
    float deltaTime;
    int screenWidth;
    int screenHeight;
    tile displayedTile[30][30];
    input input;
    inventory inventory;
} game;

#endif