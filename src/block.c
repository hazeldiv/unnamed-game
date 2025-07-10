#include <stdio.h>
#include <block.h>
#include <game.h>
#include <vec2.h>
#include <math.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

block block_registry[BLOCK_COUNT];
static block pressedBlock;
const float brokeDuration = 0.5f;
static float currentBrokeDuration = brokeDuration;
static int isDestroying, isPlacing;

void init_blocks(game *gameState) {
    block_registry[BLOCK_AIR]   = (block){ "Air", 0, {5, 224, 252}, NULL  };
    block_registry[BLOCK_GRASS]  = (block){ "Grass", 1, {11, 135, 0}, IMG_LoadTexture(gameState->renderer, "assets/dirt 2.png") };
    block_registry[BLOCK_DIRT] = (block){ "Dirt", 2, {135, 61, 0}, IMG_LoadTexture(gameState->renderer, "assets/dirt.png") };
    block_registry[BLOCK_STONE]  = (block){ "Stone", 3, {122, 122, 122}, IMG_LoadTexture(gameState->renderer, "assets/stone.png") };
    for (int y=0;y<30;y++) {
        for (int x=0;x<30;x++) {
            gameState->displayedBlock[y][x].id = -1;
        }
    }
}

void getBlockCoords(game *gameState) {
    vec2 position;
    int flag = 0;
    for (int y=0;y<30;y++) {
        for (int x=0;x<30;x++) {
            if (gameState->displayedBlock[y][x].id == -1) continue;
            if (gameState->displayedBlock[y][x].screenPosition.x > gameState->input.inputPosition.x) continue;
            if (gameState->displayedBlock[y][x].screenPosition.y > gameState->input.inputPosition.y) {
                flag = 1;
                break;
            }
            pressedBlock.id = gameState->displayedBlock[y][x].id;
            pressedBlock.coords.x = gameState->displayedBlock[y][x].coords.x;
            pressedBlock.coords.y = gameState->displayedBlock[y][x].coords.y;
        }
        if (flag) break;
    }
}

void destroy_block(game *gameState) {
    isDestroying = 1;
    currentBrokeDuration -= gameState->deltaTime;
    if (currentBrokeDuration <= 0) {
        gameState->world[(int)pressedBlock.coords.y][(int)pressedBlock.coords.x] = 0;
        pressedBlock.id = -1;
        currentBrokeDuration = brokeDuration;
    }
}

void place_block(game *gameState) {
    isPlacing = 1;
    gameState->world[(int)pressedBlock.coords.y][(int)pressedBlock.coords.x] = 2;
}

SDL_AppResult blocks_iterate(game *gameState) {
    if (gameState->input.isPressed) {
        block prevPressedBlock = pressedBlock;
        getBlockCoords(gameState);
        vec2 distance = vec2_subtract(vec2_floor(gameState->entityState.position), pressedBlock.coords);
        if (!vec2_compare(prevPressedBlock.coords, pressedBlock.coords) || abs(distance.x) > 2 || abs(distance.y) > 2) {
            currentBrokeDuration = brokeDuration;
        }
        if (pressedBlock.id != 0 && isPlacing == 0) {
            destroy_block(gameState);
        } else if (isDestroying == 0 && abs(distance.x) <= 2 && abs(distance.y) <= 2 && isPlacing == 0 && (distance.x != 0 || distance.y != 0)) {
            place_block(gameState);
        }
    } else {
        isDestroying = isPlacing = 0;
        pressedBlock.id = -1;
        currentBrokeDuration = brokeDuration;
    }
    
    return SDL_APP_CONTINUE;
}

