#ifndef ENTITY_H
#define ENTITY_H
#include <collider.h>
#include <vec2.h>
#include <SDL3/SDL.h>

typedef struct entity {
    int id;
    char name[50];
    vec2 velocity;  
    float gravity;
    vec2 position;
    rectCollider collider;
    float height;
    float width;
    int moving_right;
    int moving_left;
    int jump;
    int onGround;
    vec2 targetPos;
    vec2 startPos;
    Uint64 timeStamp;
    Uint64 prevTimeStamp;
    Uint64 currentTimeStamp;
} entity;

#endif