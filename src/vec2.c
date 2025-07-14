#include <vec2.h>
#include <math.h>

vec2 vec2_add(vec2 a, vec2 b) {
    return (vec2){a.x + b.x, a.y + b.y};
}

vec2 vec2_scale(vec2 a, float s) {
    return (vec2){a.x * s, a.y * s};
}

int vec2_compare(vec2 a, vec2 b) {
    return a.x == b.x && a.y == b.y;
}

int vec2_gt(vec2 a, vec2 b) {
    return a.x > b.x && a.y > b.y;
}

int vec2_lt(vec2 a, vec2 b) {
    return a.x < b.x && a.y < b.y;
}

vec2 vec2_floor(vec2 a) {
    return (vec2){floor(a.x), floor(a.y)};
}

vec2 vec2_subtract(vec2 a, vec2 b) {
    return (vec2){a.x - b.x, a.y - b.y};
}

vec2 vec2_abs(vec2 a) {
    return (vec2){abs(a.x), abs(a.y)};
}

int vec2_distanceCompare(vec2 a, vec2 b, int s) {
    vec2 distance = vec2_abs(vec2_subtract(a,b));
    return distance.x <= s && distance.y <= s;
}