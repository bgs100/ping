#include <algorithm>
#include "Texture.h"
#include "utility.h"

Texture::Texture() : w(0), h(0), texture(NULL) {}

Texture::Texture(SDL_Texture *texture, int w, int h) : w(w), h(h), texture(texture) {
    int *w_ptr = NULL, *h_ptr = NULL;
    if (w == -1)
        w_ptr = &this->w;
    if (h == -1)
        h_ptr = &this->h;
    if (w_ptr || h_ptr)
        SDL_QueryTexture(texture, NULL, NULL, w_ptr, h_ptr);
}

Texture::Texture(Texture &other) : w(other.w), h(other.h), texture(other.texture) {
    other.w = other.h = 0;
    other.texture = NULL;
}

Texture::Texture(Texture &&other) : w(other.w), h(other.h), texture(other.texture) {
    other.w = other.h = 0;
    other.texture = NULL;
}

Texture::~Texture() {
    if (texture != NULL && SDL_WasInit(SDL_INIT_VIDEO))
        SDL_DestroyTexture(texture);
}

Texture &Texture::operator=(Texture &&other) {
    std::swap(w, other.w);
    std::swap(h, other.h);
    std::swap(texture, other.texture);
    return *this;
}

bool Texture::empty() {
    return texture == NULL;
}

void Texture::render(SDL_Renderer *renderer, int x, int y) {
    if (texture == NULL)
        return;
    SDL_Rect dst = { x, y, w, h };
    SDL_RenderCopy(renderer, texture, NULL, &dst);
}

void Texture::render(SDL_Renderer *renderer, int srcX, int srcY, int w, int h, int dstX, int dstY) {
    if (texture == NULL)
        return;
    SDL_Rect src = { srcX, srcY, w, h };
    SDL_Rect dst = { dstX, dstY, w, h };
    SDL_RenderCopy(renderer, texture, &src, &dst);
}

void Texture::render(SDL_Renderer *renderer, int x, int y, double angle) {
    SDL_Rect dst = { x, y, w, h };
    SDL_RenderCopyEx(renderer, texture, NULL, &dst, angle, NULL, SDL_FLIP_NONE);
}

void Texture::render(SDL_Renderer *renderer, int x, int y, int w, int h, double angle) {
    SDL_Rect dst = { x, y, w, h };
    SDL_RenderCopyEx(renderer, texture, NULL, &dst, angle, NULL, SDL_FLIP_NONE);
}

Texture Texture::fromSurface(SDL_Renderer *renderer, SDL_Surface *surface) {
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL)
        SDLerror("SDL_CreateTextureFromSurface");
    return Texture(texture, surface->w, surface->h);
}

Texture Texture::fromText(SDL_Renderer *renderer, TTF_Font *font, const char *text, Uint8 r, Uint8 g, Uint8 b) {
    SDL_Color color = { r, g, b, 0xff };
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    Texture texture;
    if (surface != NULL) {
        texture = fromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    return texture;
}
