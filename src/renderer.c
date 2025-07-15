#include <stdio.h>
#include <SDL3/SDL.h>
#include <game.h>
#include <block.h>
#include <vec2.h>
#include <background.h>
#include <math.h>
#include <item.h>
#include <tile.h>
#include <SDL3_image/SDL_image.h>
#include <string.h>

int blockSize = 100;
int zoomLevel = 1;
int blockOffsetX, blockOffsetY, zoomedBlockSize, pixelOffsetX, pixelOffsetY;
int prevZoomLevel;

int coordToBlockPos(int coord) {
    return coord/blockSize;
}


SDL_AppResult renderer_init(game* gameState)
{
    SDL_SetAppMetadata("Example Renderer Rectangles", "1.0", "c./om.example.renderer-rectangles");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    gameState->window = NULL;
    gameState->renderer = NULL;

    if (!SDL_CreateWindowAndRenderer("examples/renderer/rectangles", gameState->screenWidth, gameState->screenHeight, 0, &gameState->window, &gameState->renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    zoomedBlockSize = zoomLevel*blockSize;
    blockOffsetX = (gameState->screenWidth/2-50*zoomLevel)/zoomedBlockSize;
    blockOffsetY = (gameState->screenHeight/2-50*zoomLevel)/zoomedBlockSize;
    
    pixelOffsetX = gameState->screenWidth/2-(50*zoomLevel+blockOffsetX*zoomedBlockSize);
    pixelOffsetY = gameState->screenHeight/2-(50*zoomLevel+blockOffsetY*zoomedBlockSize);

    prevZoomLevel = zoomLevel;
    return SDL_APP_CONTINUE;
}

SDL_AppResult renderer_iterate(game* gameState) {
    SDL_FRect rect;
    int i;
    int prevBlock = -1;
    int prevBackground = -1;
    for (int y=0;y<30;y++) {
        for (int x=0;x<30;x++) {
            gameState->displayedTile[y][x].block = -1;
            gameState->displayedTile[y][x].background = -1;
        }
    }
    rect.x = 0;
    rect.y = 0;
    rect.w = gameState->screenWidth;
    rect.h = gameState->screenHeight;
    SDL_SetRenderDrawColor(gameState->renderer, 5, 224, 252, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRects(gameState->renderer, &rect, 1);
    rect.w = 100;
    rect.h = 100;
    if (prevZoomLevel != zoomLevel) {
        blockOffsetX = (gameState->screenWidth/2-50*zoomLevel)/zoomedBlockSize;
        blockOffsetY = (gameState->screenHeight/2-50*zoomLevel)/zoomedBlockSize;
        pixelOffsetX = gameState->screenWidth/2-(50*zoomLevel+blockOffsetX*zoomedBlockSize);
        pixelOffsetY = gameState->screenHeight/2-(50*zoomLevel+blockOffsetY*zoomedBlockSize);
        prevZoomLevel = zoomLevel;
    }

    float position_x = gameState->entityState.position.x;
    int border = 0;
    if (gameState->entityState.position.x*100 <= gameState->screenWidth/2) {
        border = 1;
        position_x = (float)gameState->screenWidth/2/100;
    }
    if (gameState->entityState.position.x*100 >= 100*100-gameState->screenWidth/2) {
        border = 2;
        position_x = 100-(float)gameState->screenWidth/2/100;
    }

    int RectPos[gameState->screenHeight/zoomedBlockSize+3];
    for (int x=-2;x<gameState->screenWidth/zoomedBlockSize+1;x++) {
        RectPos[x+2] = x*zoomedBlockSize + pixelOffsetX - ((int)(position_x*100)%zoomedBlockSize-50)*zoomLevel;
    }
    vec2_int tileCoords;
    for (int y=-2;y<gameState->screenHeight/zoomedBlockSize+1;y++) {
        tileCoords.y = y+(int)gameState->entityState.position.y-blockOffsetY;
        rect.y = y*zoomedBlockSize + pixelOffsetY - ((int)(round(gameState->entityState.position.y*10000)/100.0f)%zoomedBlockSize-50)*zoomLevel;
        for (int x=-2;x<gameState->screenWidth/zoomedBlockSize+1;x++) {
            tileCoords.x = x+(int)position_x-blockOffsetX;
            tile tile = gameState->world[tileCoords.y][tileCoords.x];
            
            rect.x = RectPos[x+2];
            
            gameState->displayedTile[y+2][x+2] = tile;
            gameState->displayedTile[y+2][x+2].screenPosition.x = rect.x;
            gameState->displayedTile[y+2][x+2].screenPosition.y = rect.y;

            if (tile.background != ITEM_NONE) {
                SDL_RenderTexture(gameState->renderer, item_registry[tile.background].defaultTexture, NULL, &rect);
            }
            if (tile.block != ITEM_NONE) {
                SDL_RenderTexture(gameState->renderer, item_registry[tile.block].defaultTexture, NULL, &rect);
            }

            char str[5];
            sprintf(str, "%d", tileCoords.x);
            SDL_SetRenderDrawColor(gameState->renderer, 0,0,0, SDL_ALPHA_OPAQUE);
            SDL_RenderDebugText(gameState->renderer, rect.x+10, rect.y+10, str);
            sprintf(str, "%d", tileCoords.y);
            SDL_RenderDebugText(gameState->renderer, rect.x+35, rect.y+10, str);
        }
    }
    
    SDL_SetRenderDrawColor(gameState->renderer, 255,0,0, SDL_ALPHA_OPAQUE);
    rect.x = gameState->screenWidth/2-gameState->entityState.width*zoomLevel/2*blockSize;
    if (border == 1) {
        rect.x = gameState->entityState.position.x*100 - gameState->entityState.width/2*blockSize;
    } else if (border == 2) {
        rect.x = gameState->entityState.position.x*100 - (100*100 - gameState->screenWidth) - gameState->entityState.width/2*blockSize;
    }
    rect.y = gameState->screenHeight/2-gameState->entityState.height*zoomLevel/2*blockSize;
    rect.w = gameState->entityState.width*zoomLevel*blockSize; 
    rect.h = gameState->entityState.height*zoomLevel*blockSize;
    SDL_RenderFillRects(gameState->renderer, &rect, 1);
    TTF_Text *ttfText = TTF_CreateText(gameState->engine, gameState->font, gameState->entityState.name, strlen(gameState->entityState.name));
    TTF_SetTextColor(ttfText, 255,0,0, SDL_ALPHA_OPAQUE);
    TTF_DrawRendererText(ttfText, rect.x, rect.y-26);
    TTF_DestroyText(ttfText);

    for (int i=0;i<32;i++) {
        if (gameState->surroundingPlayer[i].id != 0) {
            //printf("%f %f\n", gameState->surroundingPlayer[i].position.x, gameState->surroundingPlayer[i].position.y);
            if (vec2_distanceCompare(gameState->entityState.position, gameState->surroundingPlayer[i].position, 8)) {
                
                vec2 distance = vec2_subtract(gameState->surroundingPlayer[i].position, gameState->entityState.position);
                rect.x = gameState->screenWidth/2-gameState->entityState.width*zoomLevel/2*blockSize + distance.x*blockSize;
                rect.y = gameState->screenHeight/2-gameState->entityState.height*zoomLevel/2*blockSize + distance.y*blockSize;
                rect.w = gameState->entityState.width*zoomLevel*blockSize; 
                rect.h = gameState->entityState.height*zoomLevel*blockSize;
                
                SDL_RenderFillRects(gameState->renderer, &rect, 1);
                ttfText = TTF_CreateText(gameState->engine, gameState->font, gameState->surroundingPlayer[i].name, strlen(gameState->surroundingPlayer[i].name));
                TTF_SetTextColor(ttfText, 255,0,0, SDL_ALPHA_OPAQUE);
                TTF_DrawRendererText(ttfText, rect.x, rect.y-26);
                TTF_DestroyText(ttfText);
            }
        }
    }

    return SDL_APP_CONTINUE;
}