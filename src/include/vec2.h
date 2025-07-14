#ifndef VEC2_H
#define VEC2_H

typedef struct vec2 {
    float x, y;
} vec2;

typedef struct vec2_int {
    int x, y;
} vec2_int;

vec2 vec2_add(vec2 a, vec2 b);

vec2 vec2_scale(vec2 a, float s);

int vec2_compare(vec2 a, vec2 b);

vec2 vec2_floor(vec2 a);

vec2 vec2_subtract(vec2 a, vec2 b);

int vec2_gt(vec2 a, vec2 b);

int vec2_lt(vec2 a, vec2 b);

vec2 vec2_abs(vec2 a);

int vec2_distanceCompare(vec2 a, vec2 b, int s);

#endif