#include <stdio.h>
#include <SDL3/SDL.h>
#include <game.h>
#include <collider.h>
#include <block.h>

int prev_hor = -1;
int prev_ver = -1;

int epsilon_floor(float x) {
    float epsilon = 0.001f;
    return (int) x + epsilon;
}

int check_collison(entity *entityState, game *gameState, float distance, int direction) {
    float detector[4][2] = {{entityState->position.x + entityState->collider.x, entityState->position.y + entityState->collider.y},
                            {entityState->position.x  - entityState->collider.x - 0.01f, entityState->position.y + entityState->collider.y},
                            {entityState->position.x  + entityState->collider.x, entityState->position.y - entityState->collider.y - 0.01f},
                            {entityState->position.x  - entityState->collider.x - 0.01f, entityState->position.y - entityState->collider.y - 0.01f}};
    if (direction == 1) {
        distance = distance == 0 ? 0.01f : distance;
        if (gameState->world[epsilon_floor(detector[2][1] + distance)][epsilon_floor(detector[2][0])] != 0 ||
            gameState->world[epsilon_floor(detector[3][1] + distance)][epsilon_floor(detector[3][0])] != 0) return 1;
    } else if (direction == 2) {
        if (gameState->world[epsilon_floor(detector[0][1] + distance)][epsilon_floor(detector[0][0])] != 0 ||
            gameState->world[epsilon_floor(detector[1][1] + distance)][epsilon_floor(detector[1][0])] != 0) return 1;
    } else if (direction == 3) {
        if (gameState->world[epsilon_floor(detector[0][1])][epsilon_floor(detector[0][0] + distance)] != 0 ||
            gameState->world[epsilon_floor(detector[2][1])][epsilon_floor(detector[2][0] + distance)] != 0 || detector[0][0] + distance < 0) return 1;
    } else if (direction == 4) {
        if (gameState->world[epsilon_floor(detector[1][1])][epsilon_floor(detector[1][0] + distance)] != 0 ||
            gameState->world[epsilon_floor(detector[3][1])][epsilon_floor(detector[3][0] + distance)] != 0 || detector[1][0] + distance > 100) return 1;
    }
    return 0;
}

SDL_AppResult physics_iterate(entity *entityState, game *gameState) {
    float delta_x = entityState->velocity.x*gameState->deltaTime;
    float delta_y = entityState->velocity.y*gameState->deltaTime * -1;
    int test = check_collison(entityState, gameState, 0.01f, 1);
    
    entityState->onGround = 0;
    if (check_collison(entityState, gameState, delta_y, 1)) {
        if (delta_y > 0.0f) {
            entityState->position.y += (float)round(entityState->position.y - entityState->collider.y+delta_y) - (entityState->position.y - entityState->collider.y);
            entityState->velocity.y = 0;
            delta_y = 0.0f;
        } else {
            entityState->onGround = 1;
        }
    } else {
        if (entityState->velocity.y < 0) {
            entityState->velocity.y = -8;
        } else {
            entityState->velocity.y -= 38.4f*gameState->deltaTime;
        }
        delta_y = entityState->velocity.y*gameState->deltaTime * -1;
    }
    if (check_collison(entityState, gameState, delta_y, 2) && delta_y<0) {
        entityState->position.y -= (entityState->position.y + entityState->collider.y) - (float)round(entityState->position.y + entityState->collider.y);
        entityState->velocity.y = 0;
        delta_y = 0;
    }
    if (check_collison(entityState, gameState, delta_x, 3) && delta_x < 0) {
        entityState->position.x -= (entityState->position.x + entityState->collider.x) - (float)round(entityState->position.x + entityState->collider.x);
        entityState->velocity.x = 0;
        delta_x = 0;
    }

    if (check_collison(entityState, gameState, delta_x, 4) && delta_x > 0) {
        entityState->position.x += (float)round(entityState->position.x - entityState->collider.x+delta_x) - (entityState->position.x - entityState->collider.x);
        entityState->velocity.x = 0;
        delta_x = 0;
    }

    entityState->position.x += delta_x;
    entityState->position.y += delta_y;
    return SDL_APP_CONTINUE;
}

void add_velocity(entity *entityState, float fx, float fy) {
    entityState->velocity.x = fx;
    entityState->velocity.y = fy;
}