#include <stdio.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdlib.h>
#include <game.h>
#include <block.h>
#include <collider.h>

game *gameState;

// SDL_AppResult renderer_init(void* userdata);
// SDL_AppResult renderer_event(const SDL_Event* event, void* userdata);
// SDL_AppResult renderer_iterate(void* userdata);
// SDL_AppResult entity_event(const SDL_Event* event, void* userdata);
// SDL_AppResult physics_iterate(entity *EntityState,game *gameState);

void generateWorld() {
    for (int y=0;y<100;y++) {
        for (int x=0;x<100;x++) {
            // if (x==51) {
            //     if (y == 52) gameState->world[y][x] = 2;
            //     continue;
            // };
            gameState->background[y][x] = 0;
            gameState->world[y][x] = 0;
            int chance = rand()%100;
            if (y<=50) {
                gameState->world[y][x] = 0;
                // if (y==50 && (x < 43 || x>58)) {
                //     if (chance < 50) {
                //         gameState->world[y][x] = 4;
                //     }
                // }
            } else if (y==51) {
                gameState->world[y][x] = 1;
            } else {
                gameState->world[y][x] = 2;
                if (y>60) {
                    //if (chance<3) {
                    gameState->world[y][x] = 3;
                    //}
                }
            }
            if (y==50) {
                if (x==48 || x==54) {
                    gameState->world[y][x] = 2;
                }
            }
            if (y == 50 && x == 51) gameState->background[y][x] = 1;
            if (y == 49 && x == 49) gameState->world[y][x] = 2;
        }
    }
    // for (int i=0;i<2;i++) {
    //     world[51+i][50] = 0;
    // }
}


SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    gameState = (game*)malloc(sizeof(game));
    player_init(&(gameState->entityState));
    gameState->screenWidth = 1080;
    gameState->screenHeight = 640;
    gameState->isInventoryOpen = 0;
    generateWorld();
    renderer_init(gameState);
    init_background(gameState);
    init_blocks(gameState);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if ((*event).type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    input_event(event, gameState);

    return SDL_APP_CONTINUE;
}

Uint64 start = 0;

SDL_AppResult SDL_AppIterate(void *appstate)
{
    
    physics_iterate(&(gameState->entityState), gameState);
    player_iterate(gameState, &(gameState->entityState), &(gameState->input));
    blocks_iterate(gameState);
    renderer_iterate(gameState);
    
    
    //printf("%lf\n", (double)(now-start) / 1000.0);
    //int sleep = 16 - (now-start);
    //if (sleep<0) sleep=1;
    SDL_Delay(1);
    const Uint64 now = SDL_GetTicks();

    gameState->deltaTime = (float)(now-start) / 1000.0f;
    start = now;

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    
}