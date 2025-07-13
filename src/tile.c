#include <stdio.h>
#include <block.h>
#include <game.h>
#include <vec2.h>
#include <math.h>
#include <inventory.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

block block_registry[BLOCK_COUNT];
static tile pressedTile;
const float brokeDuration = 0.5f;
static float currentBrokeDuration = brokeDuration;
static int isDestroying, isPlacing;

void getTileCoords(game *gameState) {
    vec2 position;
    int flag = 0;
    for (int y=0;y<30;y++) {
        for (int x=0;x<30;x++) {
            if (gameState->displayedTile[y][x].block == -1 && gameState->displayedTile[y][x].background == -1) continue;
            if (gameState->displayedTile[y][x].screenPosition.x > gameState->input.inputPosition.x) continue;
            if (gameState->displayedTile[y][x].screenPosition.y > gameState->input.inputPosition.y) {
                flag = 1;
                
                break;
            }
            pressedTile.block = gameState->displayedTile[y][x].block;
            pressedTile.background = gameState->displayedTile[y][x].background;
            pressedTile.coords.x = gameState->displayedTile[y][x].coords.x;
            pressedTile.coords.y = gameState->displayedTile[y][x].coords.y;
            
        }
        if (flag) break;
    }
}

void destroy_tile(game *gameState) {
    isDestroying = 1;
    currentBrokeDuration -= gameState->deltaTime;
    if (currentBrokeDuration <= 0) {
        if (gameState->world[(int)pressedTile.coords.y][(int)pressedTile.coords.x].block != ITEM_NONE) {
            gameState->world[(int)pressedTile.coords.y][(int)pressedTile.coords.x].block = ITEM_NONE;
            pressedTile.block = -1;
        } else {
            gameState->world[(int)pressedTile.coords.y][(int)pressedTile.coords.x].background = ITEM_NONE;
            pressedTile.background = -1;
        }
        currentBrokeDuration = brokeDuration;
    }
}

void place_tile(game *gameState) {
    isPlacing = 1;
    gameState->world[(int)pressedTile.coords.y][(int)pressedTile.coords.x].block = gameState->inventory.hotbarItem[gameState->inventory.selectedHotbar];
}

SDL_AppResult tile_iterate(game *gameState) {
    if (gameState->input.isPressed && !gameState->input.disabled && !gameState->input.pressingHotbar) {
        tile prevPressedTile = pressedTile;
        getTileCoords(gameState);
        vec2 distance = vec2_subtract(vec2_floor(gameState->entityState.position), pressedTile.coords);
        if (!vec2_compare(prevPressedTile.coords, pressedTile.coords) || abs(distance.x) > 2 || abs(distance.y) > 2) {
            currentBrokeDuration = brokeDuration;
        }
        if ((pressedTile.block != ITEM_NONE || pressedTile.background != ITEM_NONE) && isPlacing == 0) {
            destroy_tile(gameState);
        } else if (isDestroying == 0 && abs(distance.x) <= 2 && abs(distance.y) <= 2 && isPlacing == 0 && (distance.x != 0 || distance.y != 0)) {
            place_tile(gameState);
        }
    } else {
        isDestroying = isPlacing = 0;
        pressedTile.block = ITEM_NONE;
        pressedTile.background = ITEM_NONE;
        currentBrokeDuration = brokeDuration;
    }
    
    return SDL_APP_CONTINUE;
}