#include "Texture.h"
#include "utility.h"

Texture::Texture(SDL_Texture *texture, int w, int h) : w(w), h(h), texture(texture) {
    int *w_ptr = NULL, *h_ptr = NULL;
    if (w == -1)
        w_ptr = &this->w;
    if (h == -1)
        h_ptr = &this->h;
    if (w_ptr || h_ptr)
        SDL_QueryTexture(texture, NULL, NULL, w_ptr, h_ptr);
}

Texture::~Texture() {
    SDL_DestroyTexture(texture);
}

void Texture::render(SDL_Renderer *renderer, int x, int y) {
    SDL_Rect dst = { x, y, w, h };
    SDL_RenderCopy(renderer, texture, NULL, &dst);
}

void Texture::render(SDL_Renderer *renderer, int srcX, int srcY, int w, int h, int dstX, int dstY) {
    SDL_Rect src = { srcX, srcY, w, h };
    SDL_Rect dst = { dstX, dstY, w, h };
    SDL_RenderCopy(renderer, texture, &src, &dst);
}

Texture *Texture::fromSurface(SDL_Renderer *renderer, SDL_Surface *surface) {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL)
        SDLerror("SDL_CreateTextureFromSurface");
    return new Texture(texture, surface->w, surface->h);
}

Texture *Texture::fromText(SDL_Renderer *renderer, TTF_Font *font, const char *text, Uint8 r, Uint8 g, Uint8 b) {
    if (text[0] == '\0') // TTF_RenderText_Solid returns NULL when given empty strings.
        return NULL;
    SDL_Color color = { r, g, b, 0xff };
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    Texture *texture = fromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    return texture;
}
