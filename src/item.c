#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <background.h>
#include <game.h>
#include <item.h>
#include <block.h>

item item_registry[ITEM_COUNT];

SDL_AppResult item_init(game *gameState) {
    item_registry[ITEM_DIRT] = (item){ "Dirt", 1, (block){1}, (background){1}, ITEMTYPE_BLOCK, 1, IMG_LoadTexture(gameState->renderer, "assets/dirt.png") };
    item_registry[ITEM_STONE] = (item){ "Stone", 2, (block){2}, (background){2}, ITEMTYPE_BLOCK, 1, IMG_LoadTexture(gameState->renderer, "assets/stone.png") };
    item_registry[ITEM_PLANK] = (item){ "Plank", 3, (block){3}, (background){3}, ITEMTYPE_BLOCK, 1, NULL };
    item_registry[ITEM_PLANKWALL] = (item){ "Plank Wall", 4, (block){4}, (background){4}, ITEMTYPE_BACKGROUND, 1, IMG_LoadTexture(gameState->renderer, "assets/plank-wall.png") };

    return SDL_APP_CONTINUE;
}

