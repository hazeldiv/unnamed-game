#ifndef COMPONENT_H
#define COMPONENT_H
#include <vec2.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
// #include <stdint.h>

enum {
    ALLIGNMENT_LEFT_TOP,
    ALLIGNMENT_CENTER_TOP,
    ALLIGNMENT_RIGHT_TOP,
    ALLIGNMENT_LEFT_CENTER,
    ALLIGNMENT_CENTER_CENTER,
    ALLIGNMENT_RIGHT_CENTER,
    ALLIGNMENT_LEFT_BOTTOM,
    ALLIGNMENT_CENTER_BOTTOM,
    ALLIGNMENT_RIGHT_BOTTOM,
};

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

typedef struct text {
    
} text;

typedef struct button {
    SDL_Renderer* renderer;
    int x;
    int y;
    int w;
    int h;
    SDL_Texture *texture;
    color fill;
    color outline;
    char *text;
    TTF_TextEngine *engine;
    TTF_Font *font;
} button;

void drawButton(button button);

#endif