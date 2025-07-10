#include <stdio.h>
#include <SDL3/SDL.h>
#include <entity.h>
#include <input.h>
#include <game.h>

SDL_AppResult player_init(entity *entityState) {
    entityState->tag = "player";
    entityState->name = "player";
    entityState->width = 0.5f;
    entityState->height = 0.8f;
    entityState->velocity.x = 0.0f;
    entityState->velocity.y = 0.0f;
    entityState->jump = 0;
    entityState->collider.x = -25.0f/100.0f;
    entityState->collider.w = 50.0f/100.0f;
    entityState->collider.y = -40.0f/100.0f;
    entityState->collider.h = 80.0f/100.0f;
    entityState->position.x = (50*100 + 100/2)/100.0;
    entityState->position.y = 50 + (100)/100.0 - entityState->height/2;
}

static float speed = 5;
const float jumpCooldown = 0.3f;
static float currentJumpCooldown = jumpCooldown;

SDL_AppResult player_iterate(game *gameState, entity *entityState, input *input) {
    //printf("%d %d\n", input->movingLeft, input->movingRight);
    int movement = input->movingRight - input->movingLeft;
    entityState->velocity.x = movement*speed;
    if (currentJumpCooldown > -1) {
        currentJumpCooldown -= gameState->deltaTime;
    }
    if (input->jump) {
        if (entityState->onGround && currentJumpCooldown <= 0) {
            currentJumpCooldown = jumpCooldown;
            entityState->onGround = 0;
            entityState->velocity.y = 9.6f;
        }
    }
    
    return SDL_APP_CONTINUE;
}