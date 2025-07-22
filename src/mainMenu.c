#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <game.h>
#include <input.h>
#include <component.h>

static int render = 0;

SDL_AppResult MainMenu_event(SDL_Event *event, game *gameState) {
    if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        // int pressedComponent = getComponent((vec2){event->button.x,event->button.y}, gameState);
        // focus = 0;
        // if (pressedComponent == 1) {
        //     gameState->currentScene = 4;
            

        //     strcpy(gameState->entityState.name, inputText);
        //     SDL_Thread *thread = SDL_CreateThread((SDL_ThreadFunction)client_init, "client_init", gameState);
        //     SDL_DetachThread(thread);
        // } 
        // if (pressedComponent == 0) {
        //     SDL_StartTextInput(gameState->window);
        //     focus = 1;
        // } else {
        //     SDL_StopTextInput(gameState->window);
        // }
    } else if (event->type == SDL_EVENT_KEY_DOWN) {
        
    }
}

void mainMenu_render(game *gameState) {
    vec2 center = (vec2){gameState->screenWidth/2, gameState->screenHeight/2};
    SDL_Texture *texture = IMG_LoadTexture(gameState->renderer, "assets/main-menu-button.png");
    button button = {gameState->renderer, center.x-texture->w/2, center.y, texture->w, texture->h, texture};
    button.text = "Play";
    button.engine = gameState->engine;
    button.font = gameState->font;
    drawButton(button);

    button.y += button.h + 20;
    button.text = "Setting";
    drawButton;

    button.y += button.h + 20;
    button.text = "Quit";
    drawButton;
    SDL_DestroyTexture(button.texture);
}

void mainMenu_iterate(game *gameState) {
    if (!render) {
        mainMenu_render(gameState);
    }
}