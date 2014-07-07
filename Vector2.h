// -*- c++ -*-
#ifndef PING_VECTOR2_H
#define PING_VECTOR2_H

#include <ostream>

class Vector2 {
public:
    double x, y;

    Vector2();
    Vector2(double x, double y);

    double length();
    double sqrLength();

    Vector2 unit();
    double cross(const Vector2 &other);

    Vector2 &operator+=(const Vector2 &other);
    Vector2 &operator-=(const Vector2 &other);

    Vector2 &operator*=(double scalar);
    Vector2 &operator/=(double scalar);
};

bool operator==(const Vector2 &a, const Vector2 &b);

Vector2 operator+(Vector2 a, const Vector2 &b);
Vector2 operator-(Vector2 a, const Vector2 &b);

Vector2 operator*(Vector2 a, double b);
Vector2 operator*(double a, Vector2 b);
Vector2 operator/(Vector2 a, double b);
Vector2 operator/(double a, Vector2 b);

double operator*(const Vector2 &a, const Vector2 &b);

std::ostream &operator<<(std::ostream &out, const Vector2 &v);

#endif
