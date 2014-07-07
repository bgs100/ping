#include <algorithm>
#include "SharedState.h"
#include "GameManager.h"
#include "utility.h"

// TODO: Remove debug lines.
#include <iostream>

SharedState::SharedState(int numPlayers, /*bool walls,*/ StateListener *listener) : boundaries(numPlayers), players(numPlayers), scores(numPlayers), ball(20, 20), listener(listener), collided(-1) {
    reset();
}

void SharedState::reset() {
    if (players.size() == 0)
        return;

    // TODO: Replace with parameter.
    bool walls = false;

    int numWalls = players.size();
    if (walls)
        numWalls *= 2;

    double sideLength;
    if (numWalls % 2 == 0) {
        centerY = GameManager::HEIGHT / 2;
        sideLength = GameManager::HEIGHT * tan(pi / numWalls);
    } else {
        centerY = GameManager::HEIGHT / (1 + cos(pi / numWalls));
        sideLength = centerY * 2 * sin(pi / numWalls);
    }

    scale = 1.0;
    if (sideLength < GameManager::HEIGHT / 2)
        scale = sideLength / GameManager::HEIGHT * 2;

    double exteriorAngle = 2*pi / players.size();
    double interiorAngle = pi - exteriorAngle, theta = pi;
    Vector2 v((GameManager::WIDTH - sideLength) / 2.0, GameManager::HEIGHT-1);
    // Matches the first player with the straightest, left-most
    // boundary line.
    playerBoundaryOffset = 0;
    double straightDiff = pi / 2;

    for (unsigned int i = 0; i < players.size(); i++) {
        boundaries[i] = v;
        double diff = fabs(pi/2 - fmod(theta, pi));
        if (diff < straightDiff) {
            playerBoundaryOffset = i;
            straightDiff = diff;
        }
        theta -= pi - interiorAngle;
        v.x += sideLength * cos(theta);
        v.y -= sideLength * sin(theta);
    }

    for (unsigned int i = 0; i < players.size(); i++) {
        players[i].w = std::max(1.0, 20 * scale);
        players[i].h = std::max(1.0, 80 * scale);

        double angle = pi/2 - (i + playerBoundaryOffset) * exteriorAngle;

        Vector2 midpoint = (boundaries[(playerBoundaryOffset+i)%boundaries.size()] + boundaries[(playerBoundaryOffset+i-1)%boundaries.size()]) / 2;
        players[i].setCenter(midpoint.x + 30 * scale * cos(angle), midpoint.y - 30 * scale * sin(angle));
        players[i].theta = fmod((playerBoundaryOffset + i) * exteriorAngle, 2*pi);
        if (players[i].theta > pi)
            players[i].theta -= pi;

        players[i].v = 0;
        players[i].orientation = fmod(3*pi/2 + ((playerBoundaryOffset + i) % boundaries.size()) * exteriorAngle, 2*pi);

        scores[i] = 0;
    }

    ball.w = std::max(1.0, 20 * scale);
    ball.h = std::max(1.0, 20 * scale);
    ball.x = GameManager::WIDTH/2 - ball.w/2;
    ball.y = centerY;
    ball.theta = rand() % boundaries.size() * 2*pi / boundaries.size() + pi/2 - pi / boundaries.size();
    ball.v = 3 * scale;

    score1 = score2 = 0;
}

void SharedState::update(std::vector<int> inputs) {
    // TODO: Break up into multiple methods?
    for (unsigned int i = 0; i < players.size(); i++) {
        int min = -10, max = 10;
        int change = inputs[i];
        if (abs(change + players[i].v) < abs(players[i].v)) {
            change *= 2;
            if (players[i].v > 0)
                min = 0;
            else
                max = 0;
        }

        players[i].v = clamp(players[i].v + change * scale, min, max);
        players[i].update();

        // Check paddle for collisions with neighboring boundaries.
        for (int b = -1; b < 3; b += 2) {
            Vector2 start = boundaries[(boundaries.size()+i+b+playerBoundaryOffset-1) % boundaries.size()];
            Vector2 end = boundaries[(i+b+playerBoundaryOffset) % boundaries.size()];
            std::vector<Vector2> vertices = players[i].getVertices();

            for (unsigned int v = 0; v < vertices.size(); v++) {
                // The following bit of magic detects which side of the
                // boundary the vertex is on.
                // (https://stackoverflow.com/questions/1560492/how-to-tell-whether-a-point-is-to-the-right-or-left-side-of-a-line)
                if (((end.x-start.x)*(vertices[v].y-start.y) - (end.y-start.y)*(vertices[v].x-start.x)) < 0) {
                    players[i].v = 0;
                    int other;
                    if (v == 0 || v == 3)
                        other = 3 - v;
                    else
                        other = 2 - v + 1;

                    // (https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect)
                    Vector2 p = start;
                    Vector2 r = end - p;
                    Vector2 q = vertices[other];
                    Vector2 s = vertices[v] - q;
                    
                    // The -.01 pulls the intersection back very
                    // slightly, so that it isn't still past the
                    // border.
                    double u = (q - p).cross(r) / (r.cross(s)) - .01;
                    Vector2 intersection = q + u * s;
                    players[i].setCenter(players[i].getCenter() - vertices[v] + intersection);
                }
            }
        }

        // Check paddle for collision with neighboring paddles.
        // This only runs for every other paddle.
        for (int j = -1; (i % 2 == 0) && (j < 2); j += 2) {
            Entity &other = players[(players.size()+j+(int)i)%players.size()];
            std::vector<Vector2> projections;
            if (players[i].collide(other, &projections)) {
                std::cout << "bonk between " << i << " and " << j+(int)i << std::endl;

                std::cout << "projections: ";
                for (auto proj = projections.begin(); proj < projections.end(); ++proj)
                    std::cout << *proj << " ";
                std::cout << std::endl;

                std::cout << "unit: ";
                for (auto proj = projections.begin(); proj < projections.end(); ++proj)
                    std::cout << proj->unit() << " ";
                std::cout << std::endl;

                Vector2 axis1 = (players[i].getVertices()[0] - players[i].getVertices()[3]).unit();
                Vector2 axis2 = (other.getVertices()[0] - other.getVertices()[3]).unit();

                std::cout << "axis1 " << axis1 << "; axis2 " << axis2 << std::endl;

                Vector2 projected;
                Vector2 proj1;
                Vector2 proj2;

                for (auto proj = projections.begin(); proj < projections.end(); ++proj) {
                    if ((proj->unit() - axis1).length() < (proj1.unit() - axis1).length())
                        proj1 = *proj;
                    if ((proj->unit() - axis2).length() < (proj2.unit() - axis2).length())
                        proj2 = *proj;
                    if (projected.length() == 0 || proj->length() < projected.length())
                        projected = *proj;
                }

                std::cout << "projected: " << projected << "; unit: " << projected.unit() << std::endl;

                if ((projected.unit() * axis1) != 0)
                    proj1 = fabs(projected.length() / (projected.unit() * axis1)) * axis1;
                else
                    proj1 = axis1 * (projected * axis1);
                std::cout << "(projected.length() / (projected.unit() * axis1)) = " << (projected.length() / (projected.unit() * axis1)) << std::endl;

                std::cout << "(projected.unit() * axis2) = " << (projected.unit() * axis2) << std::endl;
                if ((projected.unit() * axis2) != 0)
                    proj2 = fabs(projected.length() / (projected.unit() * axis2)) * axis2;
                else
                    proj2 = axis2 * (projected * axis2);

                std::cout << "proj1 " << proj1 << "; proj2 " << proj2 << std::endl;

                double playerV = fabs(players[i].v * axis1 * projected.unit());
                double otherV = fabs(other.v * axis2 * projected.unit());
                double totalV = playerV + otherV;
                std::cout << playerV << " + " << otherV << " = " << totalV << std::endl;

                if (j > 0)
                    proj1 *= -1;
                else
                    proj2 *= -1;

                proj1 *= playerV / totalV * 1.01;
                proj2 *= otherV / totalV * 1.01;

                std::cout << "NEW proj1 " << proj1 << "; proj2 " << proj2 << std::endl;

                players[i].x += proj1.x;
                players[i].y += proj1.y;
                other.x += proj2.x;
                other.y += proj2.y;

                players[i].v = 0;
                other.v = 0;

                //assert(!players[i].collide(other));
            }
        }
    }

    Entity oldBall(ball);
    ball.update();

    // TODO: Add new boundary wall-bounce code after adding walls parameter.
    /*if (ball.y < 0 || ball.y + ball.h > GameManager::HEIGHT) {
        if (listener != NULL)
            listener->onBounce();
        ball.theta = 2*pi - ball.theta;
        // Like with the paddle bouncing code below, extra distance is
        // re-applied, so the ball travels at a consistent rate
        // despite bouncing.
        if (ball.y < 0)
            ball.y = ball.getDY() - oldBall.y;
        else
            ball.y = GameManager::HEIGHT - ball.h + ball.getDY() + (GameManager::HEIGHT - ball.h) - oldBall.y;
    }*/

    // TODO: Replace with line (from paddle sides) and OBB
    // intersection check (project box and line on to perpindicular
    // line).

    bool anyCollision = false;

    for (unsigned int i = 0; i < players.size(); i++) {
        bool collision = ball.collide(players[i]);
        anyCollision |= collision;
        // TODO: Re-add phasing fix.

        if ((collided == -1 || collided != (int)i) && collision) {
            if (listener != NULL)
                listener->onHit();
            ball.theta = 2*players[i].theta - ball.theta;
            ball.setDelta(ball.getDX() + players[i].getDX() / 2, ball.getDY() + players[i].getDY() / 2);
            ball.v *= 1.1;
            collided = i;
        }
    }

    if (collided != -1 && !anyCollision)
        collided = -1;

    for (auto p = players.begin(); p < players.end(); ++p) {
        if (p->v > 0)
            p->v = clamp(p->v - .1, 0, 10);
        else
            p->v = clamp(p->v + .1, -10, 0);
    }

    bool scored = false;
    bool anyThrough[scores.size()];

    for (unsigned int i = 0; i < boundaries.size(); i++) {
        Vector2& start = boundaries[(boundaries.size()+i-1)%boundaries.size()];
        Vector2& end = boundaries[i];
        std::vector<Vector2> vertices = ball.getVertices();
        int scoreIndex = (scores.size() + i - playerBoundaryOffset) % scores.size();
        anyThrough[scoreIndex] = false;
        bool allThrough = true;
        for (auto v = vertices.begin(); v < vertices.end(); ++v) {
            if (((end.x-start.x)*(v->y-start.y) - (end.y-start.y)*(v->x-start.x)) > 0) {
                allThrough = false;
                if (anyThrough[scoreIndex])
                    break;
            } else {
                anyThrough[scoreIndex] = true;
                if (!allThrough)
                    break;
            }
        }

        if (allThrough)
            scored = true;
    }

    if (scored) {
        // TODO: Make it go to the last player to hit it, or any
        // direction away from those who missed.
        ball.theta = rand() % boundaries.size() * 2*pi / boundaries.size() + pi/2 - pi / boundaries.size();
        ball.v = 3 * scale;
        ball.x = GameManager::WIDTH / 2 - ball.w / 2;
        ball.y = centerY;

        for (unsigned int i = 0; i < scores.size(); i++) {
            if (!anyThrough[i])
                scores[i] += 1;
        }
    }
}
