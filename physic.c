#include <stdio.h>
#include <Interface/Game.h>

struct State {
  float accumulated[2] = {0,0};
  float acceleration[2] = {0,0};
  float velocity[2] = {0,0};  
  float gravity = 1;
  float friction = 1;
  float mass = 1;
  float pos[2];
};

float forceUpdate(struct State, float deltaTime) {
    force.acceleration[0] = force.accumulated[0]/force.mass;
    force.acceleration[1] = force.accumulated[1]/force.mass;
    force.velocity[0] = force.acceleration[0]*deltaTime;
    force.velocity[1] = force.acceleration[0]*deltaTime;
    
    if (force.accumulated[0]>0) {
        if (force.accumulated[0]>force.friction) {
            force.accumulated[0] -= force.friction;
        } else force.accumulated[0] = 0
    } else if (force.accumulated[0]<0) {
        if (force.accumulated[0]*-1>force.friction) {
            force.accumulated[0] += force.friction;
        } else force.accumulated[0] = 0
    }
    return 
    
    // if (force.accumulated[1]>0) {
    //     force.accumulated[1] -= force.gravity;
    //     if (force.accumulated<0) force.accumulated[1] = 0
    // }
}

void update(float deltaTime) {
    forceUpdate(forceUpdate)
}