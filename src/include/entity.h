#ifndef ENTITY_H
#define ENTITY_H
#include <collider.h>
#include <vec2.h>

typedef struct entity {
    vec2 velocity;  
    float gravity;
    vec2 position;
    char *name;
    char *tag;
    rectCollider collider;
    float height;
    float width;
    int moving_right;
    int moving_left;
    int jump;
    int onGround;
} entity;

#endif