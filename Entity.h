// -*- c++ -*-
#ifndef PING_ENTITY_H
#define PING_ENTITY_H

#include <math.h>
#include <SDL2/SDL.h>

class Entity {
public:
    double x, y;
    int w, h;
    double dX, dY;
    Entity(double x, double y, int w, int h, double dX=0, double dY=0) : x(x), y(y), w(w), h(h), dX(dX), dY(dY) {}
    void draw(SDL_Renderer *renderer) {
        SDL_Rect r = { (int)round(x), (int)round(y), w, h };
        SDL_RenderFillRect(renderer, &r);
    }
};

#endif
