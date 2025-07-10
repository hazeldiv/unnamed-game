#ifndef VEC2_H
#define VEC2_H

typedef struct vec2 {
    float x, y;
} vec2;

vec2 vec2_add(vec2 a, vec2 b);

vec2 vec2_scale(vec2 a, float s);

int vec2_compare(vec2 a, vec2 b);

vec2 vec2_floor(vec2 a);

vec2 vec2_subtract(vec2 a, vec2 b);

#endif