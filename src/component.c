#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <component.h>
#include <string.h>

void drawButton(button button) {
    SDL_FRect rect = {button.x, button.y, button.w, button.h};
    if (button.texture) {
        SDL_RenderTexture(button.renderer, button.texture, NULL, &rect);
    } else {
        SDL_SetRenderDrawColor(button.renderer, button.fill.r, button.fill.g, button.fill.b, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRects(button.renderer, &rect, 1);
    }

    if (button.text[0] != '\0') {
        TTF_Text *ttfText;
        ttfText = TTF_CreateText(button.engine, button.font, button.text, strlen(button.text));
        int textWidth;
        int textHeight;
        TTF_GetTextSize(ttfText, &textWidth, &textHeight);
        SDL_FRect tmp = rect;
        rect.x = rect.x + rect.w/2 - textWidth/2;
        rect.y = rect.y + rect.h/2 - textHeight/2 - 6;
        TTF_SetTextColor(ttfText, 255,255,255, SDL_ALPHA_OPAQUE);
        TTF_DrawRendererText(ttfText, rect.x, rect.y);
        TTF_DestroyText(ttfText);
    }
}