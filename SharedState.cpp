#include <algorithm>
#include <assert.h>
#include "SharedState.h"
#include "GameManager.h"
#include "utility.h"

// listener is NULL by default (see SharedState.h).
SharedState::SharedState(StateListener *listener) : listener(listener) {
}

SharedState::SharedState(int numPlayers, int wallsPerPlayer, StateListener *listener) : listener(listener) {
    reset(numPlayers, wallsPerPlayer);
}

std::vector<Entity *> SharedState::getEntities() {
    std::vector<Entity *> entities(players.size() + 1);
    entities[0] = &ball;
    for (unsigned int i = 0; i < players.size(); i++)
        entities[i+1] = &players[i];

    return entities;
}

void SharedState::resetBall() {
    ball.w = ball.h = 20 * scale;
    ball.x = GameManager::WIDTH/2 - ball.w/2;
    ball.y = centerY;
    int boundaryIndex = playerToBoundaryIndex(rand() % players.size()) - 1;
    Vector2 &boundary = boundaries[(boundaries.size() + boundaryIndex) % boundaries.size()];
    double startAngle = atan2(boundary.y - ball.y, boundary.x - ball.x);
    ball.theta = startAngle + rand() / (double)RAND_MAX * 2*pi / boundaries.size();
    ball.v = 3 * scale;
}

void SharedState::resetClassic() {
    players.resize(2);
    scores.resize(2);
    boundaries.resize(4);

    collided = -1;
    centerY = GameManager::HEIGHT / 2;
    scale = 1.0;
    playerBoundaryOffset = 1;

    boundaries[0] = Vector2(-1, GameManager::HEIGHT);
    boundaries[1] = Vector2(-1, -1);
    boundaries[2] = Vector2(GameManager::WIDTH, -1);
    boundaries[3] = Vector2(GameManager::WIDTH, GameManager::HEIGHT);

    for (int i = 0; i < 2; i++) {
        players[i].w = 20;
        players[i].h = 80;
        players[i].setCenter(30 + i * (GameManager::WIDTH - 60), GameManager::HEIGHT / 2);
        players[i].theta = pi/2;
        players[i].orientation = i * pi;
    }

    for (int &score : scores)
        score = 0;

    resetBall();
}

void SharedState::reset(int numPlayers, int wallMult) {
    players.resize(numPlayers);
    scores.resize(numPlayers);
    collided = -1;

    int numWalls = players.size() * wallMult;

    boundaries.resize(numWalls);

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
        scale *= sideLength / GameManager::HEIGHT * 2;

    double exteriorAngle = 2*pi / boundaries.size();
    double interiorAngle = pi - exteriorAngle, theta = pi;
    Vector2 v((GameManager::WIDTH - sideLength) / 2.0, GameManager::HEIGHT-1);
    // Matches the first player with the straightest, left-most
    // boundary line.
    playerBoundaryOffset = 0;
    double straightDiff = pi / 2;

    for (unsigned int i = 0; i < boundaries.size(); i++) {
        boundaries[i] = v;
        double diff = fabs(pi/2 - fmod(theta, pi));
        if ((straightDiff - diff) > (straightDiff / 1e10)) {
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

        int boundaryIndex = playerToBoundaryIndex(i);
        double angle = pi/2 - boundaryIndex * exteriorAngle;

        Vector2 midpoint = (boundaries[boundaryIndex] + boundaries[(boundaries.size()+boundaryIndex-1) % boundaries.size()]) / 2;
        players[i].setCenter(midpoint.x + 30 * scale * cos(angle), midpoint.y - 30 * scale * sin(angle));
        players[i].theta = fmod(boundaryIndex * exteriorAngle, 2*pi);
        if (players[i].theta >= pi)
            players[i].theta -= pi;

        players[i].v = 0;
        players[i].orientation = fmod(3*pi/2 + boundaryIndex * exteriorAngle, 2*pi);

        scores[i] = 0;
    }

    for (int &score : scores)
        score = 0;

    resetBall();
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

        // This is necessary to prevent the boundary check's halting
        // from interfering with paddle/paddle collision resolution.
        bool haltPlayer = false;

        // Check paddle for collisions with neighboring boundaries.
        for (int b = -1; b < 3; b += 2) {
            Vector2 start = boundaries[(boundaries.size()+playerToBoundaryIndex(i)+b-1) % boundaries.size()];
            Vector2 end = boundaries[(boundaries.size()+playerToBoundaryIndex(i)+b) % boundaries.size()];
            std::vector<Vector2> vertices = players[i].getVertices();

            for (unsigned int v = 0; v < vertices.size(); v++) {
                // The following bit of magic detects which side of the
                // boundary the vertex is on.
                // (https://stackoverflow.com/questions/1560492/how-to-tell-whether-a-point-is-to-the-right-or-left-side-of-a-line)
                if (((end.x-start.x)*(vertices[v].y-start.y) - (end.y-start.y)*(vertices[v].x-start.x)) < 0) {
                    haltPlayer = true;
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
                    
                    double u = (q - p).cross(r) / (r.cross(s));
                    Vector2 intersection = q + u * s;
                    Vector2 dir = s.unit();
                    double diff = vertices[v] * dir - intersection * dir + .01;

                    players[i].x -= (dir * diff).x;
                    players[i].y -= (dir * diff).y;
                }
            }
        }

        // Check paddle for collision with neighboring paddles.
        // This only runs for every other paddle.
        for (int j = -1; (i % 2 == 0) && (j + (int)i < (int)players.size()) && (j < 2); j += 2) {
            Entity &other = players[(players.size()+j+(int)i)%players.size()];
            std::vector<Vector2> projections;

            if (players[i].collide(other, &projections)) {
                Vector2 axis1 = (players[i].getVertices()[0] - players[i].getVertices()[3]).unit();
                Vector2 axis2 = (other.getVertices()[0] - other.getVertices()[3]).unit();

                Vector2 projected;

                for (auto proj = projections.begin(); proj < projections.end(); ++proj) {
                    if (projected.length() == 0 || proj->length() < projected.length())
                        projected = *proj;
                }

                Vector2 proj1(0, 0), proj2(0, 0);

                bool perpendicular1 = fabs(projected * axis1) < 0.0000000001;
                bool perpendicular2 = fabs(projected * axis2) < 0.0000000001;

                if (!perpendicular1)
                    proj1 = projected.length() / fabs(projected.unit() * axis1) * axis1;

                if (!perpendicular2)
                    proj2 = projected.length() / fabs(projected.unit() * axis2) * axis2;

                assert(!(isnan(proj1.x) || isnan(proj1.y) || isnan(proj2.x) || isnan(proj2.y)));

                Vector2 movement1(cos(players[i].theta), sin(players[i].theta)), movement2(cos(other.theta), sin(other.theta));

                double playerV = std::max(0.0, fabs(axis1 * projected.unit()) * movement1 * axis1 * players[i].v * (j > 0 ? 1 : -1));
                double otherV = std::max(0.0, fabs(axis2 * projected.unit()) * movement2 * axis2 * other.v * (j > 0 ? -1 : 1));
                double totalV = playerV + otherV;

                // This prevents NaN from popping up below; it should
                // only execute when we also execute 
                if (totalV == 0) {
                    totalV = 2;
                    if (!perpendicular1 && !perpendicular2) {
                        // This should never happen.
                        // But after maybe 7 hours of the game running
                        // continuously, as per a test executable that
                        // was set to just run this simulation with no
                        // window as fast as possible (~84x regular
                        // speed), it can apparently happen. So this
                        // deals with it.
                        playerV = otherV = 1;
                    }
                }

                if (j > 0)
                    proj1 *= -1;
                else
                    proj2 *= -1;

                proj1 *= playerV / totalV * 1.01;
                proj2 *= otherV / totalV * 1.01;

                assert(!(isnan(proj1.x) || isnan(proj1.y) || isnan(proj2.x) || isnan(proj2.y)));

                // Might be good to replace Entity.x/y with a position
                // vector; it'd make this two whole lines shorter!
                players[i].x += proj1.x;
                players[i].y += proj1.y;
                other.x += proj2.x;
                other.y += proj2.y;

                if (players[i].collide(other, &projections)) {
                    // This handles the case in which the shortest
                    // projection was perpendicular to one of the
                    // paddles' movement axes, and yet that paddle was
                    // moving (which occurs when the paddle would've
                    // hit the other paddle's short edge, but moved
                    // too much in a single step, instead resulting in
                    // the other paddle "hitting" the first one's
                    // broad side).

                    if (perpendicular1)
                        projected = projections[3];
                    else if (perpendicular2)
                        projected = projections[1];
                    else
                        assert(false);

                    proj1 = fabs(projected.length() / (projected.unit() * axis1)) * axis1;
                    proj2 = fabs(projected.length() / (projected.unit() * axis2)) * axis2;

                    playerV = std::max(0.0, fabs(axis1 * projected.unit()) * movement1 * axis1 * players[i].v * (j > 0 ? 1 : -1));
                    otherV = std::max(0.0, fabs(axis2 * projected.unit()) * movement2 * axis2 * other.v * (j > 0 ? -1 : 1));
                    totalV = playerV + otherV;

                    assert(totalV != 0);

                    if (j > 0)
                        proj1 *= -1;
                    else
                        proj2 *= -1;

                    proj1 *= playerV / totalV * 1.01;
                    proj2 *= otherV / totalV * 1.01;

                    assert(!(isnan(proj1.x) || isnan(proj1.y) || isnan(proj2.x) || isnan(proj2.y)));

                    players[i].x += proj1.x;
                    players[i].y += proj1.y;
                    other.x += proj2.x;
                    other.y += proj2.y;

                    assert(!players[i].collide(other));
                }

                if (playerV > 0.0000000001)
                    players[i].v = 0;
                if (otherV > 0.0000000001)
                    other.v = 0;
            }
        }

        if (haltPlayer)
            players[i].v = 0;
    }

    Entity oldBall(ball);
    ball.update();

    // TODO: Re-add phasing fix, possibly with line (from paddle
    // sides) and OBB intersection check (project ball and line on to
    // perpindicular line).

    bool anyCollision = false;

    for (unsigned int i = 0; i < players.size(); i++) {
        bool collision = ball.collide(players[i]);
        anyCollision |= collision;

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
    bool allThrough;
    bool anyThrough[scores.size()];

    // Check for 1) scoring and 2) wall bouncing.
    for (unsigned int i = 0; i < boundaries.size(); i++) {
        Vector2& start = boundaries[(boundaries.size()+i-1)%boundaries.size()];
        Vector2& end = boundaries[i];
        std::vector<Vector2> vertices = ball.getVertices();

        int playerIndex = boundaryToPlayerIndex(i);
        if (playerIndex != -1) {
            allThrough = true;
            anyThrough[playerIndex] = false;
        }

        for (const auto &v : vertices) {
            if (((end.x-start.x)*(v.y-start.y) - (end.y-start.y)*(v.x-start.x)) > 0) {
                if (playerIndex != -1) {
                    allThrough = false;
                    if (anyThrough[playerIndex])
                        break;
                }
            } else {
                if (playerIndex != -1) {
                    anyThrough[playerIndex] = true;
                    if (!allThrough)
                        break;
                } else {
                    listener->onBounce();
                    Vector2 wall = (end - start).unit();
                    Vector2 perpendicular(-wall.y, wall.x);
                    double diff = (start * perpendicular) - (v * perpendicular);

                    Vector2 oldDir(cos(ball.theta), sin(ball.theta));
                    oldDir *= diff / (oldDir * perpendicular);
                    ball.x += oldDir.x;
                    ball.y += oldDir.y;

                    double angle = atan2(wall.y, wall.x);
                    ball.theta = 2*angle - ball.theta;

                    Vector2 dir(cos(ball.theta), sin(ball.theta));
                    dir *= diff / (dir * perpendicular);
                    ball.x += dir.x;
                    ball.y += dir.y;

                    break;
                }
            }
        }

        if (playerIndex != -1 && allThrough)
            scored = true;
    }

    if (scored) {
        resetBall();

        for (unsigned int i = 0; i < scores.size(); i++) {
            if (!anyThrough[i])
                scores[i] += 1;
        }
    }
}

int SharedState::playerToBoundaryIndex(int playerIndex) const {
    return (playerBoundaryOffset + (boundaries.size() / players.size()) * playerIndex) % boundaries.size();
}

int SharedState::boundaryToPlayerIndex(int boundaryIndex) const {
    int val = (boundaries.size() + boundaryIndex - playerBoundaryOffset) % boundaries.size();
    int wallMult = boundaries.size() / players.size();
    // Check if it actually represents a player's wall.
    if ((val % wallMult) != 0)
        return -1;
    return val / wallMult;
}
