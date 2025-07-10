
#ifndef BLOCK_H
#define BLOCK_H
#include <SDL3/SDL.h>
#include <vec2.h>

typedef enum {
    BLOCK_AIR,
    BLOCK_GRASS,
    BLOCK_DIRT,
    BLOCK_STONE,
    BLOCK_COUNT
} blockType;

typedef struct block {
    const char* name;
    int id;
    int color[3];
    SDL_Texture *texture;
    vec2 screenPosition;
    vec2 coords;
    vec2 size;
} block;

extern block block_registry[BLOCK_COUNT];

#endif