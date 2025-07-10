
#ifndef BACKGROUND_H
#define BACKGROUND_H
#include <SDL3/SDL.h>
#include <vec2.h>

typedef enum {
    BACKGROUND_AIR,
    BACKGROUND_PLANK,
    BACKGROUND_COUNT
} backgroundType;

typedef struct background {
    const char* name;
    int id;
    int color[3];
    SDL_Texture *texture;
    vec2 screenPosition;
    vec2 coords;
    vec2 size;
} background;

extern background background_registry[BACKGROUND_COUNT];

#endif