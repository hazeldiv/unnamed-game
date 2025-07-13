#ifndef ITEM_H
#define ITEM_H
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <vec2.h>
#include <block.h>
#include <background.h>

typedef enum {
    ITEM_NONE,
    ITEM_DIRT,
    ITEM_STONE,
    ITEM_PLANK,
    ITEM_PLANKWALL,
    ITEM_COUNT
} itemID;

typedef enum {
    ITEMTYPE_BLOCK,
    ITEMTYPE_BACKGROUND,
    ITEMTYPE_MISC,
} itemType;

typedef struct item {
    char *name;
    int id;
    block block;
    background background;
    Uint8 itemType;
    Uint8 isPlaceable;
    SDL_Texture *defaultTexture;
    SDL_Texture *texture[5];
    vec2 screenPosition;
} item;

extern item item_registry[ITEM_COUNT];

#endif