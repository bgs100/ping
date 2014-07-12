// -*- c++ -*-
#ifndef PING_TEXTURE_H
#define PING_TEXTURE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Texture {
public:
    Texture();
    Texture(SDL_Texture *texture, int w=-1, int h=-1);
    Texture(Texture &other);
    Texture(Texture &&other);
    ~Texture();

    Texture &operator=(Texture &&other);

    bool empty();

    void setAlphaMod(Uint8 alpha);
    void setColorMod(Uint8 r, Uint8 g, Uint8 b);

    void render(SDL_Renderer *renderer, int x, int y);
    void render(SDL_Renderer *renderer, int srcX, int srcY, int w, int h, int dstX, int dstY);
    void render(SDL_Renderer *renderer, int x, int, double angle);
    void render(SDL_Renderer *renderer, int x, int y, int w, int h, double angle);

    int w, h;

    static Texture fromSurface(SDL_Renderer *renderer, SDL_Surface *surface);
    static Texture fromText(SDL_Renderer *renderer, TTF_Font *font, const char *text, Uint8 r=0xff, Uint8 g=0xff, Uint8 b=0xff);

private:
    SDL_Texture *texture;
};

#endif
