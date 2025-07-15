#include <stdio.h>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdlib.h>
#include <game.h>
#include <block.h>
#include <collider.h>
#include <item.h>
#include <inventory.h>
#include <ws2tcpip.h>
#include <tile.h>

game *gameState;

SDL_AppResult input_event(const SDL_Event* event, game *gameState);

SDL_AppResult player_init(entity *entityState);
SDL_AppResult renderer_init(game *gameState);
SDL_AppResult inventory_init(inventory *inventory);
void client_init(game *gameState);
SDL_AppResult item_init(game *gameState);

void client_iterate(game *gameState);
void client_test(game *gameState);
SDL_AppResult physics_iterate(entity *entityState, game *gameState);
SDL_AppResult player_iterate(game *gameState, entity *entityState, input *input);
SDL_AppResult tile_iterate(game *gameState);
SDL_AppResult renderer_iterate(game *gameState);
SDL_AppResult inventory_iterate(game *gameState, inventory *inventory);

void client_quit();

void generateWorld() {
    for (int y=0;y<100;y++) {
        for (int x=0;x<100;x++) {
            // if (x==51) {
            //     if (y == 52) gameState->world[y][x] = 2;
            //     continue;
            // };
            // gameState->background[y][x] = 0;
            // gameState->world[y][x] = 0;

            gameState->world[y][x].block = ITEM_NONE;
            gameState->world[y][x].background = ITEM_NONE;
            gameState->world[y][x].coords = (vec2){x,y};
            int chance = rand()%100;
            if (y>50) {
                gameState->world[y][x].block = ITEM_DIRT;
                if (y>60) {
                    gameState->world[y][x].block = ITEM_STONE;
                }
            }
            // if (y<=50) {
            //     gameState->world[y][x] = 0;
            //     // if (y==50 && (x < 43 || x>58)) {
            //     //     if (chance < 50) {
            //     //         gameState->world[y][x] = 4;
            //     //     }
            //     // }
            // } else if (y==51) {
            //     gameState->world[y][x] = 1;
            // } else {
            //     gameState->world[y][x] = 2;
            //     if (y>60) {
            //         //if (chance<3) {
            //         gameState->world[y][x] = 3;
            //         //}
            //     }
            // }
            // if (y==50) {
            //     if (x==48 || x==54) {
            //         gameState->world[y][x] = 2;
            //     }
            // }
            // if (y == 50 && x == 51) gameState->background[y][x] = 1;
            // if (y == 49 && x == 49) gameState->world[y][x] = 2;
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
    gameState->input.disabled = 0;
    gameState->input.pressingHotbar = 0;
    gameState->inventory.isOpen = 0;
    gameState->inventory.slotCount = 13;
    gameState->isClientBusy = 0;
    gameState->t = 0.0f;
    //generateWorld();


    renderer_init(gameState);
    inventory_init(&gameState->inventory);
    gameState->inventory.inventoryItem[0] = 1;
    gameState->inventory.inventoryItem[1] = 2;
    gameState->inventory.inventoryItem[2] = 4;
    gameState->inventory.inventoryItem[3] = 3;
    for (int i=0;i<32;i++) {
        gameState->surroundingPlayer[i].id = 0;
        gameState->surroundingPlayer[i].velocity = (vec2){0,0};
        gameState->surroundingPlayer[i].position.x = (50*100 + 100/2)/100.0;
        gameState->surroundingPlayer[i].position.y = 50 + (100)/100.0 - gameState->entityState.height/2;
        gameState->surroundingPlayer[i].targetPos = gameState->surroundingPlayer[i].position;
        gameState->surroundingPlayer[i].startPos = gameState->surroundingPlayer[i].position;
    }
    //background_init(gameState);
    //block_init(gameState);
    client_init(gameState);
    client_iterate(gameState);
    item_init(gameState);
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
Uint64 counter = 100;

DWORD WINAPI ClientHandler(LPVOID lpParam) {
    game *temp = (game*)lpParam;
    client_iterate(temp);
}
Uint64 prevTimeStamp;
Uint64 timeStamp;
SDL_AppResult SDL_AppIterate(void *appstate)
{   
    if (counter >= 100 && !gameState->isClientBusy) {
        gameState->isClientBusy = 1;
        CreateThread(NULL, 0, ClientHandler, gameState, 0, NULL);
        counter = 0;
        gameState->t = 0;
        
        for (int i=0;i<32;i++) {
            if (gameState->surroundingPlayer[i].id != 0) {
                printf("test %f %f\n", gameState->surroundingPlayer[i].position.x, gameState->surroundingPlayer[i].position.y, gameState->surroundingPlayer[i].targetPos.x, gameState->surroundingPlayer[i].targetPos.y);
            }
        }
        prevTimeStamp = timeStamp;
        timeStamp = SDL_GetTicks();
    }
    gameState->t = (float)counter/(timeStamp-prevTimeStamp);
    // client_test(gameState);
    physics_iterate(&(gameState->entityState), gameState);
    player_iterate(gameState, &(gameState->entityState), &(gameState->input));
    tile_iterate(gameState);
    renderer_iterate(gameState);
    inventory_iterate(gameState, &(gameState->inventory));
    SDL_RenderPresent(gameState->renderer);
    
    //printf("%lf\n", (double)(now-start) / 1000.0);
    //int sleep = 16 - (now-start);
    //if (sleep<0) sleep=1;
    SDL_Delay(1);
    const Uint64 now = SDL_GetTicks();

    gameState->deltaTime = (float)(now-start) / 1000.0f;
    counter += now-start;
    start = now;
    
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    client_quit();
}