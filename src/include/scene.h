#ifndef SCENE_H
#define SCENE_H
#include <component.h>

typedef struct scene {
    int id;
    int componentCount;
    component component[10];
} scene;



#endif