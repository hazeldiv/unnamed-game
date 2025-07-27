#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <game.h>
#include <input.h>
#include <component.h>
#define MAX_BUTTON 3

static int render = 0;
static button menuButton[MAX_BUTTON];

int getButton(vec2 position) {
    for (int i=0;i<MAX_BUTTON;i++) {
        if (position.x >= menuButton[i].x && 
            position.x <= menuButton[i].x + menuButton[i].w &&
            position.y >= menuButton[i].y && 
            position.y <= menuButton[i].y + menuButton[i].h) {

            return i;
        }
    }
    return -1;
}

SDL_AppResult MainMenu_event(SDL_Event *event, game *gameState) {
    if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        int pressedButton = getButton((vec2){event->button.x,event->button.y});
        switch (pressedButton) {
            case 0: {
                gameState->currentScene = 3;
                break;
            }
            case 1: {

                break;
            }
            case 2: {
                SDL_Quit();
                break;
            }
        }
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        // printf("%d\n", getButton((vec2){event->button.x,event->button.y}));
    }
}

void mainMenu_render(game *gameState) {
    SDL_Texture *texture = IMG_LoadTexture(gameState->renderer, "assets/main-menu-background.png");
    SDL_FRect rect = {0, 0, gameState->screenWidth, gameState->screenHeight};
    SDL_RenderTexture(gameState->renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);

    vec2 center = (vec2){gameState->screenWidth/2, gameState->screenHeight/2};
    texture = IMG_LoadTexture(gameState->renderer, "assets/main-menu-button.png");
    TTF_Font *font = TTF_OpenFont("assets/Exo2.ttf", 72.0f);
    menuButton[0] = (button){gameState->renderer, center.x-texture->w/2, center.y-200, texture->w, texture->h, texture};
    menuButton[0].text = "Play";
    menuButton[0].engine = gameState->engine;
    menuButton[0].font = font;
    drawButton(menuButton[0]);

    menuButton[1] = menuButton[0];
    menuButton[1].y = menuButton[0].y +  menuButton[0].h + 20;
    menuButton[1].text = "Setting";
    drawButton(menuButton[1]);

    menuButton[2] = menuButton[0];
    menuButton[2].y = menuButton[1].y +  menuButton[1].h + 20;
    menuButton[2].text = "Quit";
    drawButton(menuButton[2]);
    SDL_DestroyTexture(menuButton[0].texture);
    TTF_CloseFont(font);
}

void mainMenu_iterate(game *gameState) {
    mainMenu_render(gameState);
}