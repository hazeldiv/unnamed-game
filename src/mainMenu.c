#include <game.h>
#include <scene.h>
#include <component.h>
#include <vec2.h>
#include <stdio.h>
#include <string.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdlib.h>
#include <client.h>

static int id = 2;



SDL_AppResult mainMenu_init(game *gameState) {
    int h = 50;
    int w = 300;
    int gap = 10;
    int offset = (w+h+gap)/2;
    gameState->scene[id].id = id;
    gameState->scene[id].component[0].x = gameState->screenWidth/2 - offset;
    gameState->scene[id].component[0].y = gameState->screenHeight/2-h/2;
    gameState->scene[id].component[0].w = w;
    gameState->scene[id].component[0].h = h;
    gameState->scene[id].component[0].fill = (color){200,200,200};
    gameState->scene[id].component[0].outline = (color){50,50,200};

    gameState->scene[id].component[1].x = gameState->screenWidth/2 - offset + w + gap;
    gameState->scene[id].component[1].y = gameState->screenHeight/2-h/2;
    gameState->scene[id].component[1].w = h;
    gameState->scene[id].component[1].h = h;
    gameState->scene[id].component[1].fill = (color){200,200,200};
    gameState->scene[id].component[1].outline = (color){50,50,200};
    gameState->scene[id].componentCount = 2;
    
    
}

int getComponent(vec2 pos, game *gameState) {
    for (int i=0;i<gameState->scene[id].componentCount;i++) {
        vec2 comp_pos = (vec2){gameState->scene[id].component[i].x, gameState->scene[id].component[i].y};
        if (vec2_gt(pos, comp_pos) && 
            vec2_lt(pos, vec2_add(comp_pos, (vec2){gameState->scene[id].component[i].w, gameState->scene[id].component[i].h}))) {
            return i;
        }
    }
    return -1;
}
#define MAX_TEXT 21
int focus = 0;
char inputText[MAX_TEXT] = "";



SDL_AppResult mainMenu_event(SDL_Event *event, game *gameState) {
    if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        int pressedComponent = getComponent((vec2){event->button.x,event->button.y}, gameState);
        focus = 0;
        if (pressedComponent == 1) {
            strcpy(gameState->entityState.name, inputText);
            client_init(gameState);
            client_iterate(gameState);
            gameState->currentScene = 1;
        } 
        if (pressedComponent == 0) {
            SDL_StartTextInput(gameState->window);
            focus = 1;
        } else {
            SDL_StopTextInput(gameState->window);
        }
    } else if (event->type == SDL_EVENT_TEXT_INPUT) {
        if (focus && strlen(inputText) + strlen(event->text.text) < MAX_TEXT - 1) {
            strcat(inputText, event->text.text);
        }
    } else if (event->type == SDL_EVENT_KEY_DOWN) {
        if (event->key.key == SDLK_BACKSPACE) {
            inputText[strlen(inputText) - 1] = '\0';
        }
    }
}

SDL_AppResult mainMenu_iterate(game *gameState) {
    SDL_FRect rect;
    
    for (int i=0;i<gameState->scene[id].componentCount;i++) {
        
        rect.x = gameState->scene[id].component[i].x;
        rect.y = gameState->scene[id].component[i].y;
        rect.w = gameState->scene[id].component[i].w;
        rect.h = gameState->scene[id].component[i].h;
        
        color color;
        color = gameState->scene[id].component[i].fill;
        SDL_SetRenderDrawColor(gameState->renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRects(gameState->renderer, &rect, 1);
        color = gameState->scene[id].component[i].outline;
        SDL_SetRenderDrawColor(gameState->renderer, color.r, color.g, color.b, SDL_ALPHA_OPAQUE);
        SDL_RenderRect(gameState->renderer, &rect);
        if (i==0) {
            TTF_Text *ttfText;
            if (inputText[0] != '\0') {
                ttfText = TTF_CreateText(gameState->engine, gameState->font, inputText, strlen(inputText));
                TTF_SetTextColor(ttfText, 0,0,0, SDL_ALPHA_OPAQUE);
                TTF_DrawRendererText(ttfText, rect.x+20, rect.y + (50-24)/2);
                TTF_DestroyText(ttfText);
            }
            ttfText = TTF_CreateText(gameState->engine, gameState->font, "Name :", strlen("Name :"));
            TTF_SetTextColor(ttfText, 255,255,255, SDL_ALPHA_OPAQUE);
            TTF_DrawRendererText(ttfText, rect.x+20, rect.y - 30);
            TTF_DestroyText(ttfText);
        }
    }
    


    return SDL_APP_CONTINUE;
}