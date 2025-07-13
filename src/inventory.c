#include <game.h>
#include <vec2.h>
#include <inventory.h>
#include <input.h>
#include <item.h>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

void inventory_init(inventory* inventory) {
    for (int i=0;i<inventory->slotCount;i++) {
        inventory->inventoryItem[i] = 0;
    }
    for (int i=0;i<inventory->hotbarCount;i++) {
        inventory->hotbarItem[i] = 0;
    }
    inventory->hotbarCount = 5;
    inventory->hotbarSize = 100;
    inventory->selectedHotbar = -1;
}

void inventory_renderer(game *gameState, inventory* inventory) {
    SDL_FRect rect;
    SDL_FRect rectItem;
    int i;
    int itemOffset = 10;
    if (inventory->isOpen) {
        SDL_Texture *invBg = IMG_LoadTexture(gameState->renderer, "assets/inventory-background.png");
        vec2 invOffset = (vec2){(gameState->screenWidth - invBg->w)/2, 20};
        rect.x = invOffset.x;
        rect.y = invOffset.y;
        rect.w = invBg->w;
        rect.h = invBg->h;
        SDL_RenderTexture(gameState->renderer, invBg, NULL, &rect);
        SDL_Texture *invItem = IMG_LoadTexture(gameState->renderer, "assets/inventory-slot.png");
        rect.w = invItem->w;
        rect.h = invItem->h;
        rectItem.w = rect.w - itemOffset * 2;
        rectItem.h = rect.h - itemOffset * 2;
        
        int col = 6;
        int row = 1;
        inventory->slotSize = invItem->w;
        
        vec2 invItemOffset = {invOffset.x+30, invOffset.y + 30};//gameState->screenWidth/2-col*invItem->w/2
        for (i=0;i<inventory->slotCount;i++) {
            if (i%col == 0 && i!=0) {
                invItemOffset.x = invOffset.x+30;
                invItemOffset.y += invItem->h;
                row++;
            }
            rect.x = invItemOffset.x;
            rect.y = invItemOffset.y;
            inventory->slotScreenPosition[i] = (vec2) { rect.x, rect.y };
            SDL_RenderTexture(gameState->renderer, invItem, NULL, &rect);
            if (inventory->inventoryItem != 0) {
                SDL_Texture *itemTexture = item_registry[inventory->inventoryItem[i]].defaultTexture;
                rectItem.x = rect.x + itemOffset;
                rectItem.y = rect.y + itemOffset;
                SDL_RenderTexture(gameState->renderer, itemTexture, NULL, &rectItem);
            }
            
            invItemOffset.x += invItem->w;
            
        }
    }
    vec2 hotbarOffset = {gameState->screenWidth/2-inventory->hotbarSize*inventory->hotbarCount/2, gameState->screenHeight-inventory->hotbarSize-20};
    rect.w = inventory->hotbarSize;
    rect.h = inventory->hotbarSize;
    rectItem.w = rect.w - itemOffset * 2;
    rectItem.h = rect.h - itemOffset * 2;
    for (i=0;i<inventory->hotbarCount;i++) {
        rect.x = hotbarOffset.x + inventory->hotbarSize*i;
        rect.y = hotbarOffset.y;
        
        SDL_SetRenderDrawBlendMode(gameState->renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(gameState->renderer, 150,150,150,200);
        SDL_RenderFillRects(gameState->renderer, &rect, 1);
        SDL_SetRenderDrawColor(gameState->renderer, 50,50,50, SDL_ALPHA_OPAQUE);
        SDL_RenderRect(gameState->renderer, &rect);
        inventory->hotbarScreenPosition[i].x = rect.x;
        inventory->hotbarScreenPosition[i].y = rect.y;
        if (inventory->hotbarItem != 0) {
            rectItem.x = rect.x + itemOffset;
            rectItem.y = rect.y + itemOffset;
            SDL_Texture *itemTexture = item_registry[inventory->hotbarItem[i]].defaultTexture;
            SDL_RenderTexture(gameState->renderer, itemTexture, NULL, &rectItem);
        }
    }
}

int getPressedSlot(input input, inventory *inventory) {
    for (int i=0;i<inventory->slotCount;i++) {
        if (vec2_gt(input.inputPosition, inventory->slotScreenPosition[i]) && vec2_lt(input.inputPosition, vec2_add(inventory->slotScreenPosition[i], (vec2) {inventory->slotSize, inventory->slotSize}))) {
            return i;
        }
    }
    return -1;
}

int getPressedHotbar(input input, inventory *inventory) {
    for (int i=0;i<inventory->hotbarCount;i++) {
        if (vec2_gt(input.inputPosition, inventory->hotbarScreenPosition[i]) && 
            vec2_lt(input.inputPosition, vec2_add(inventory->hotbarScreenPosition[i], (vec2){inventory->hotbarSize, inventory->hotbarSize}))) {
            return i;
        }
    }
    return -1;
}

static int selectedSlot = -1;

SDL_AppResult inventory_iterate(game *gameState, inventory *inventory) {
    inventory_renderer(gameState, inventory);
    if (gameState->input.isPressed) {
        int pressedHotbar = getPressedHotbar(gameState->input, inventory);
        if (pressedHotbar != -1) {
            inventory->selectedHotbar = pressedHotbar;
            selectedSlot = -1;
        }
        if (inventory->isOpen) {
            int pressedSlot = getPressedSlot(gameState->input, inventory);
            if (pressedSlot != -1) {
                selectedSlot = pressedSlot;
            }

            if (inventory->selectedHotbar != -1 && selectedSlot != -1) {
                inventory->hotbarItem[inventory->selectedHotbar] = inventory->inventoryItem[selectedSlot];
                selectedSlot = -1;
                inventory->selectedHotbar = -1;
            }
        }
    }
    

    return SDL_APP_CONTINUE;
}