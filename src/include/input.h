#ifndef INPUT_H
#define INPUT_H
#include <vec2.h>

typedef struct input {
    int movingLeft;
    int movingRight;
    int jump;
    vec2 inputPosition;
    int isPressed;
    int disabled;
    int pressingHotbar;
} input;

#endif