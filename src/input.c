#include <SDL3/SDL.h>
#include <game.h>
#include <inventory.h>

int input_validation(SDL_Event *event, int disabled) {
    if (disabled) {
        if (event->key.key == SDLK_A ||
            event->key.key == SDLK_D ||
            event->key.key == SDLK_SPACE) {
                return 1;
        }
    }
    return 0;
}

void checkPressedInput(game *gameState) {
    if (getPressedHotbar(gameState->input, &gameState->inventory) != -1) {
        gameState->input.pressingHotbar = 1;
    } else {
        gameState->input.pressingHotbar = 0;
    }
}

SDL_AppResult input_event(SDL_Event *event, game *gameState) {
    if (input_validation(event, gameState->input.disabled)) {
        return SDL_APP_CONTINUE;
    }
    if (event->type == SDL_EVENT_KEY_DOWN) {
        Uint32 key = event->key.key;
        switch (key) {
            case SDLK_A:
                gameState->input.movingLeft = 1;
                break;
            case SDLK_D:
                gameState->input.movingRight = 1;
                break;
            case SDLK_SPACE:
                if (!gameState->input.jump) gameState->input.jump = 1;
                break;
        }
    } else if (event->type == SDL_EVENT_KEY_UP) {
        Uint32 key = event->key.key;
        switch (key) {
            case SDLK_A:
                gameState->input.movingLeft = 0;
                break;
            case SDLK_D:
                gameState->input.movingRight = 0;
                break;
            case SDLK_SPACE:
                gameState->input.jump = 0;
                break;
            case SDLK_E:
                gameState->input.disabled = !gameState->input.disabled;
                if (gameState->input.disabled) {
                    gameState->input.movingLeft = 0;
                    gameState->input.movingRight = 0;
                }
                gameState->inventory.isOpen = !gameState->inventory.isOpen;
                break;
        }
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        gameState->input.inputPosition.x = event->button.x;
        gameState->input.inputPosition.y = event->button.y;
        gameState->input.isPressed = 1;
        checkPressedInput(gameState);
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
        gameState->input.inputPosition.x = -1;
        gameState->input.inputPosition.y = -1;
        gameState->input.isPressed = 0;
    } else if (event->type == SDL_EVENT_MOUSE_MOTION) {
        gameState->input.inputPosition.x = event->motion.x;
        gameState->input.inputPosition.y = event->motion.y;
    }
    return SDL_APP_CONTINUE;
}

