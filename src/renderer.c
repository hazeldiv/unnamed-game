#include <stdio.h>
#include <SDL3/SDL.h>
#include <game.h>
#include <block.h>
#include <vec2.h>
#include <background.h>
#include <math.h>

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

    rect.x = 0;
    rect.y = 0;
    rect.w = gameState->screenWidth;
    rect.h = gameState->screenHeight;
    SDL_SetRenderDrawColor(gameState->renderer, 255,100,0, SDL_ALPHA_OPAQUE);
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
    for (int y=-2;y<gameState->screenHeight/zoomedBlockSize+1;y++) {
        int currentBlockY = y+(int)gameState->entityState.position.y-blockOffsetY;
        rect.y = y*zoomedBlockSize + pixelOffsetY - ((int)(round(gameState->entityState.position.y*10000)/100.0f)%zoomedBlockSize-50)*zoomLevel;
        for (int x=-2;x<gameState->screenWidth/zoomedBlockSize+1;x++) {
            int currentBlockX = x+(int)position_x-blockOffsetX;
            block *currentBlock = &block_registry[gameState->world[currentBlockY][currentBlockX]];
            background *currentBackground = &background_registry[gameState->background[currentBlockY][currentBlockX]];
            
            // if (currentBackground->id!=prevBackground) {
            //     SDL_SetRenderDrawColor(gameState->renderer, currentBackground->color[0], currentBackground->color[1], currentBackground->color[2], SDL_ALPHA_OPAQUE);
            //     prevBackground = currentBackground->id;
            // }
            // if (currentBlock->id!=prevBlock) {
            //     SDL_SetRenderDrawColor(gameState->renderer, currentBlock->color[0], currentBlock->color[1], currentBlock->color[2], SDL_ALPHA_OPAQUE);
            //     prevBlock = currentBlock->id;
            // }


            rect.x = RectPos[x+2];
            gameState->displayedBlock[y+2][x+2].screenPosition.x = rect.x;
            gameState->displayedBlock[y+2][x+2].screenPosition.y = rect.y;
            gameState->displayedBlock[y+2][x+2].coords.x = currentBlockX;
            gameState->displayedBlock[y+2][x+2].coords.y = currentBlockY;
            gameState->displayedBlock[y+2][x+2].id = currentBlock->id;

            if (currentBackground->texture != NULL) {
                SDL_RenderTexture(gameState->renderer, currentBackground->texture, NULL, &rect);
            } else {
                SDL_SetRenderDrawColor(gameState->renderer, currentBackground->color[0], currentBackground->color[1], currentBackground->color[2], SDL_ALPHA_OPAQUE);
                SDL_RenderFillRects(gameState->renderer, &rect, 1);
            }
            if (currentBlock->id != 0) {
                if (currentBlock->texture != NULL) {
                    SDL_RenderTexture(gameState->renderer, currentBlock->texture, NULL, &rect);
                } else {
                    SDL_SetRenderDrawColor(gameState->renderer, currentBlock->color[0], currentBlock->color[1], currentBlock->color[2], SDL_ALPHA_OPAQUE);
                    SDL_RenderFillRects(gameState->renderer, &rect, 1);
                }
            }

            char str[5];
            itoa(currentBlockY, str, 10);
            SDL_SetRenderDrawColor(gameState->renderer, 0,0,0, SDL_ALPHA_OPAQUE);
            SDL_RenderDebugText(gameState->renderer, rect.x+10, rect.y+10, str);
            itoa(currentBlockX, str, 10);
            SDL_RenderDebugText(gameState->renderer, rect.x+35, rect.y+10, str);
            SDL_SetRenderDrawColor(gameState->renderer, currentBlock->color[0], currentBlock->color[1], currentBlock->color[2], SDL_ALPHA_OPAQUE);
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

    if (gameState->isInventoryOpen) {
        rect.x = 20;
        rect.y = 20;
        rect.w = gameState->screenWidth-40;
        rect.h = gameState->screenHeight-40;
        SDL_SetRenderDrawColor(gameState->renderer, 150,150,150, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRects(gameState->renderer, &rect, 1);
    } else {
        int hotbarSlotSize = 100;
        int hotbarSlot = 5;
        vec2 hotbarOffset = {gameState->screenWidth/2-hotbarSlotSize*hotbarSlot/2, gameState->screenHeight-hotbarSlotSize-20};
        for (int i=0;i<hotbarSlot;i++) {
            rect.x = hotbarOffset.x + hotbarSlotSize*i;
            rect.y = hotbarOffset.y;
            rect.w = hotbarSlotSize;
            rect.h = hotbarSlotSize;
            SDL_SetRenderDrawBlendMode(gameState->renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(gameState->renderer, 150,150,150,200);
            SDL_RenderFillRects(gameState->renderer, &rect, 1);
            SDL_SetRenderDrawColor(gameState->renderer, 50,50,50, SDL_ALPHA_OPAQUE);
            SDL_RenderRect(gameState->renderer, &rect);
        }
    }
    

    SDL_RenderPresent(gameState->renderer);

    return SDL_APP_CONTINUE;
}