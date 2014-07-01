#include "SharedState.h"
#include "GameManager.h"
#include "utility.h"

SharedState::SharedState(StateListener *listener) : player(20, 80), opponent(20, 80), ball(20, 20), listener(listener), collided(false) {
    reset();
}

void SharedState::reset() {
    player.x = 20;
    player.y = GameManager::HEIGHT/2 - player.h/2;
    player.theta = pi/2;
    player.v = 0;
    player.orientation = 0;

    opponent.x = GameManager::WIDTH - 20 - opponent.w;
    opponent.y = GameManager::HEIGHT/2 - opponent.h/2;
    opponent.theta = pi/2;
    opponent.v = 0;
    opponent.orientation = 0;

    ball.x = GameManager::WIDTH/2 - ball.w/2;
    ball.y = GameManager::HEIGHT/2 - ball.h/2;
    ball.theta = rand() % 2 * pi / 2 + pi/4;
    ball.v = rand() % 2 * 6 - 3;

    score1 = score2 = 0;
}

void SharedState::update(int pInput, int oInput) {
    Entity *paddles[] = { &player, &opponent };
    int inputs[] = { pInput, oInput };
    for (int i = 0; i < 2; i++) {
        int min = -10, max = 10;
        int change = inputs[i];
        if (abs(change + paddles[i]->v) < abs(paddles[i]->v)) {
            change *= 2;
            if (paddles[i]->v > 0)
                min = 0;
            else
                max = 0;
        }
        paddles[i]->v = clamp(paddles[i]->v + change, min, max);
    }

    player.update();
    if (player.y < 0 || player.y + player.h > GameManager::HEIGHT) {
        player.y = clamp(player.y, 0, GameManager::HEIGHT-player.h);
        player.v = 0;
    }
    opponent.update();
    if (opponent.y < 0 || opponent.y + opponent.h > GameManager::HEIGHT) {
        opponent.y = clamp(opponent.y, 0, GameManager::HEIGHT-opponent.h);
        opponent.v = 0;
    }

    double oldX = ball.x, oldY = ball.y;
    ball.update();
    if (ball.y < 0 || ball.y + ball.h > GameManager::HEIGHT) {
        if (listener != NULL)
            listener->onBounce();
        ball.theta = 2*pi - ball.theta;
        // Like with the paddle bouncing code below, extra distance is
        // re-applied, so the ball travels at a consistent rate
        // despite bouncing.
        if (ball.y < 0)
            ball.y = ball.getDY() - oldY;
        else
            ball.y = GameManager::HEIGHT - ball.h + ball.getDY() + (GameManager::HEIGHT - ball.h) - oldY;
    }

    bool which, collisionPlayer = ball.collide(player), collisionOpponent = ball.collide(opponent);
    // This is in case the ball's movement speed is greater than the
    // width of a paddle, which would otherwise allow the ball to
    // phase through it. It checks if the ball went completely through
    // the paddle's area, and if so "moves" the ball along its path to
    // the paddle's x axis and checks for collision there.
    if ((which = (oldX > player.x + player.w && ball.x < player.x)) || (oldX < opponent.x && ball.x > opponent.x + opponent.w)) {
        if (which) {
            double intersectY = ball.getSlope() * (player.x + player.w - oldX) + ball.y;
            Entity testBall(player.x + player.w, intersectY, ball.w, ball.h, 0, 0);
            collisionPlayer |= testBall.collide(player);
        } else {
            double intersectY = ball.getSlope() * (opponent.x - ball.w - oldX) + ball.y;
            Entity testBall(opponent.x - ball.w, intersectY, ball.w, ball.h, 0, 0);
            collisionOpponent |= testBall.collide(opponent);
        }
    }

    // The collided flag prevents the ball from colliding with the
    // same paddle multiple times in one hit; this is an issue, for
    // instance, when a paddle hits a ball with one of its smaller
    // side edges (as the ball may not move out of the way quickly enough).
    if (!collided && (collisionPlayer || collisionOpponent)) {
        if (listener != NULL)
            listener->onHit();
        ball.setDX(clamp(ball.getDX() * -1.1, -923, 923));
        if (collisionPlayer) {
            ball.setDY(clamp(ball.getDY() + player.getDY() / 2, -747, 747));
            if (ball.collide(player)) {
                collided = true;
            } else {
                // If the ball hits the paddle fast enough to go
                // through it, the extra distance (that it would have
                // gone if it hadn't hit the paddle) is reapplied in
                // the ball's new direction.
                double dXremainder = ball.getDX() - oldX + player.x + player.w;
                ball.x = player.x + player.w + dXremainder;
            }
        } else {
            ball.setDY(clamp(ball.getDY() + opponent.getDY() / 2, -747, 747));
            if (ball.collide(opponent)) {
                collided = true;
            } else {
                double dXremainder = ball.getDX() + opponent.x - ball.w - oldX;
                ball.x = opponent.x - ball.w + dXremainder;
            }
        }
    } else if (collided && !collisionPlayer && !collisionOpponent) {
        collided = false;
    }

    if (player.v > 0)
        player.v = clamp(player.v - .1, 0, 10);
    else
        player.v = clamp(player.v + .1, -10, 0);

    if (opponent.v > 0)
        opponent.v = clamp(opponent.v - .1, 0, 10);
    else
        opponent.v = clamp(opponent.v + .1, -10, 0);

    if (ball.x + ball.w < 0 || ball.x > GameManager::WIDTH) {
        if (ball.x + ball.w < 0) {
            score2++;
            ball.theta = rand() % 2 * 3*pi/2 + pi/4;
        } else {
            score1++;
            ball.theta = rand() % 2 * pi/2 + 3*pi/4;
        }
        ball.v = 3;
        ball.x = GameManager::WIDTH / 2 - ball.w / 2;
        ball.y = GameManager::HEIGHT / 2 - ball.h / 2;
    }
}
