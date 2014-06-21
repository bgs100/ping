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
    void render(SDL_Renderer *renderer, double extrapolation=0) {
        SDL_Rect r = { (int)round(x + extrapolation*dX), (int)round(y + extrapolation*dY), w, h };
        SDL_RenderFillRect(renderer, &r);
    }
};

#endif
