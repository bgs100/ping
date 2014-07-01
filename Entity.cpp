#include "Entity.h"

Vector2::Vector2(double x, double y) : x(x), y(y) {}

Vector2 Vector2::unit() {
    double magnitude = sqrt(x*x + y*y);
    return Vector2(x / magnitude, y / magnitude);
}

Vector2 &Vector2::operator-=(const Vector2 &other) {
        x -= other.x;
        y -= other.y;
        return *this;
}

Vector2 operator-(Vector2 a, const Vector2 &b) {
    return a -= b;
}

double operator*(const Vector2 &a, const Vector2 &b) {
    return a.x * b.x + a.y * b.y;
}

Entity::Entity() : x(0), y(0), w(0), h(0), theta(0), v(0), orientation(0) {}

Entity::Entity(int w, int h) : x(0), y(0), w(w), h(h), theta(0), v(0), orientation(0) {}

// theta and v are 0 by default (see Entity.h).
Entity::Entity(double x, double y, int w, int h, double theta, double v, double orientation)
    : x(x), y(y), w(w), h(h), theta(theta), v(v), orientation(orientation) {
}

double Entity::getDX() const {
    return v * cos(theta);
}

double Entity::getDY() const {
    return v * sin(theta);
}

double Entity::getSlope() const {
    return tan(theta);
}

Vector2 Entity::getCenter() const {
    return Vector2(x + w/2, y + h/2);
}

std::vector<Vector2> Entity::getVertices() const {
    Vector2 c = getCenter();
    return {
        Vector2(c.x - cos(orientation) * w/2 + sin(orientation) * h/2, c.y - sin(orientation) * w/2 - cos(orientation) * h/2),
        Vector2(c.x + cos(orientation) * w/2 + sin(orientation) * h/2, c.y + sin(orientation) * w/2 - cos(orientation) * h/2),
        Vector2(c.x + cos(orientation) * w/2 - sin(orientation) * h/2, c.y + sin(orientation) * w/2 + cos(orientation) * h/2),
        Vector2(c.x - cos(orientation) * w/2 - sin(orientation) * h/2, c.y - sin(orientation) * w/2 + cos(orientation) * h/2)
    };
}

void Entity::setDelta(double dX, double dY) {
    theta = atan2(dY, dX);
    v = sqrt(dX*dX + dY*dY);
}

void Entity::setDX(double dX) {
    setDelta(dX, getDY());
}

void Entity::setDY(double dY) {
    setDelta(getDX(), dY);
}

bool Entity::collide(const Entity &other) const {
    if (orientation == 0 && other.orientation == 0)
        return (y + h >= other.y && y <= other.y + other.h && x + w >= other.x && x <= other.x + other.w);

    std::vector<Vector2> vertices = getVertices();
    std::vector<Vector2> otherVertices = other.getVertices();

    Vector2 axis[4] = { (vertices[0] - vertices[1]).unit(), (vertices[0] - vertices[3]).unit(),
                        (otherVertices[0] - otherVertices[1]).unit(), (otherVertices[0] - otherVertices[3]).unit() };

    for (int a = 0; a < 4; a++) {
        double min[2] = { vertices[0] * axis[a], otherVertices[0] * axis[a] };
        double max[2] = { vertices[0] * axis[a], otherVertices[0] * axis[a] };
        
        for (int v = 1; v < 4; v++) {
            double prod = vertices[v] * axis[a];
            double otherProd = otherVertices[v] * axis[a];
            
            min[0] = std::min(prod, min[0]);
            max[0] = std::max(prod, max[0]);

            min[1] = std::min(otherProd, min[1]);
            max[1] = std::max(otherProd, max[1]);
        }
        
        if (min[0] > max[1] || min[1] > max[0])
            return false;
    }

    return true;
}

void Entity::update() {
    x += getDX();
    y += getDY();
}
