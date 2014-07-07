#include <math.h>
#include "Vector2.h"

Vector2::Vector2() : x(0), y(0) {}

Vector2::Vector2(double x, double y) : x(x), y(y) {}

double Vector2::length() {
    return sqrt(x*x + y*y);
}

// For comparison purposes.
double Vector2::sqrLength() {
    return x*x + y*y;
}

Vector2 Vector2::unit() {
    double magnitude = length();
    if (magnitude == 0)
        return Vector2();
    return Vector2(x / magnitude, y / magnitude);
}

double Vector2::cross(const Vector2 &other) {
    return x * other.y - y * other.x;
}

Vector2 &Vector2::operator+=(const Vector2 &other) {
    x += other.x;
    y += other.y;
    return *this;
}

Vector2 &Vector2::operator-=(const Vector2 &other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

Vector2 &Vector2::operator*=(double scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
}

Vector2 &Vector2::operator/=(double scalar) {
    x /= scalar;
    y /= scalar;
    return *this;
}

bool operator==(const Vector2 &a, const Vector2 &b) {
    return a.x == b.x && a.y == b.y;
}

Vector2 operator+(Vector2 a, const Vector2 &b) {
    return a += b;
}

Vector2 operator-(Vector2 a, const Vector2 &b) {
    return a -= b;
}

Vector2 operator*(Vector2 a, double b) {
    return a *= b;
}

Vector2 operator*(double a, Vector2 b) {
    return b *= a;
}

Vector2 operator/(Vector2 a, double b) {
    return a /= b;
}

Vector2 operator/(double a, Vector2 b) {
    return b /= a;
}

double operator*(const Vector2 &a, const Vector2 &b) {
    return a.x * b.x + a.y * b.y;
}

std::ostream &operator<<(std::ostream &out, const Vector2 &v) {
    out << "(" << v.x << ", " << v.y << ")";
    return out;
}
