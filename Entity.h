// -*- c++ -*-
#ifndef PING_ENTITY_H
#define PING_ENTITY_H

#include <vector>
#include <math.h>
#include <SDL2/SDL.h>

// TODO: Maaaybe move this to its own file? Not convinced; Entity is
// the only class that actually uses it, and it's pretty bare bones.
class Vector2 {
public:
    double x, y;
    Vector2(double x, double y);

    Vector2 unit();
    Vector2 &operator-=(const Vector2 &other);
};

Vector2 operator-(Vector2 a, const Vector2 &b);
double operator*(const Vector2 &a, const Vector2 &b);

class Entity {
public:
    double x, y;
    int w, h;
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

    bool collide(const Entity &other) const;

    void update();
};

#endif
