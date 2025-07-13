#ifndef TILE_H
#define TILE_H
#include <SDL3/SDL.h>
#include <vec2.h>
#include <item.h>

typedef struct tile {
    int block;
    int background;
    vec2 screenPosition;
    vec2 coords;
} tile;

#endif