// -*- c++ -*-
#ifndef PING_TEXTBOX_H
#define PING_TEXTBOX_H

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "Texture.h"

class Textbox {
public:
    std::string text;

    Textbox(TTF_Font *font, int x, int y, int w);
    ~Textbox();

    void clear();

    bool handleEvent(SDL_Event &event);
    void render(SDL_Renderer *renderer);

private:
    TTF_Font *font;
    int x, y, w, h;
    Texture texture;
    bool renderText;
    unsigned int pos;
    int offsetX;
};

#endif
