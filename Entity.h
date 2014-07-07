// -*- c++ -*-
#ifndef PING_ENTITY_H
#define PING_ENTITY_H

#include <vector>
#include <math.h>
#include <SDL2/SDL.h>
#include "Vector2.h"

class Entity {
public:
    double x, y;
    int w, h;
    // NOTE: Angles are in radians, and go clockwise from +X (due to
    // +Y being down).
    double theta, v;
    double orientation;

    Entity();
    Entity(int w, int h);
    Entity(double x, double y, int w, int h, double theta=0, double v=0, double orientation=0);

    double getDX() const;
    double getDY() const;
    double getSlope() const;

    Vector2 getCenter() const;

    std::vector<Vector2> getVertices() const;

    void setDelta(double dX, double dY);
    void setDX(double dX);
    void setDY(double dY);

    void setCenter(double cX, double cY);
    void setCenter(const Vector2 &c);

    bool collide(const Entity &other, std::vector<Vector2> *projections=NULL) const;

    void update();
};

#endif
