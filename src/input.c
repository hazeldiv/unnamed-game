#include <SDL3/SDL.h>
#include <game.h>

SDL_AppResult input_event(SDL_Event *event, game *gameState) {
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
                gameState->isInventoryOpen = !gameState->isInventoryOpen;
                break;
        }
    } else if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        gameState->input.inputPosition.x = event->button.x;
        gameState->input.inputPosition.y = event->button.y;
        gameState->input.isPressed = 1;
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