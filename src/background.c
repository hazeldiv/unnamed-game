#include <stdio.h>
#include <background.h>
#include <game.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

background background_registry[BACKGROUND_COUNT];

void init_background(game *gameState) {
    background_registry[BACKGROUND_AIR]  = (background){ "Air", 0, {5, 224, 252}, NULL  };
    background_registry[BACKGROUND_PLANK] = (background){ "Plank", 1, {11, 135, 0}, IMG_LoadTexture(gameState->renderer, "assets/plank-wall.png") };
}

