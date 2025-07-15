#ifndef COMPONENT_H
#define COMPONENT_H
#include <vec2.h>
#include <SDL3_image/SDL_image.h>

typedef struct color {
    int r;
    int g;
    int b;
} color;

typedef struct component {
    int x;
    int y;
    int w;
    int h;
    SDL_Texture *texture;
    color fill;
    color outline;
} component;

#endif