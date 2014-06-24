#include "SharedState.h"
#include "GameManager.h"
#include "utility.h"

SharedState::SharedState(StateListener *listener) : player(20, 80), opponent(20, 80), ball(20, 20), listener(listener), collided(false) {
    reset();
}

void SharedState::reset() {
    player.x = 20;
    player.y = GameManager::HEIGHT/2 - player.h/2;
    player.dX = player.dY = 0;

    opponent.x = GameManager::WIDTH - 20 - opponent.w;
    opponent.y = GameManager::HEIGHT/2 - opponent.h/2;
    opponent.dX = opponent.dY = 0;

    ball.x = GameManager::WIDTH/2 - ball.w/2;
    ball.y = GameManager::HEIGHT/2 - ball.h/2;
    ball.dX = rand() % 2 * 4 - 2;
    ball.dY = rand() % 2 * 4 - 2;

    score1 = score2 = 0;
}

void SharedState::update(int pInput, int oInput) {
    Entity *paddles[] = { &player, &opponent };
    int inputs[] = { pInput, oInput };
    for (int i = 0; i < 2; i++) {
        int min = -10, max = 10;
        int change = inputs[i];
        if (abs(change + paddles[i]->dY) < abs(paddles[i]->dY)) {
            change *= 2;
            if (paddles[i]->dY > 0)
                min = 0;
            else
                max = 0;
        }
        paddles[i]->dY = clamp(paddles[i]->dY + change, min, max);
    }

    player.y += player.dY;
    if (player.y < 0 || player.y + player.h > GameManager::HEIGHT) {
        player.y = clamp(player.y, 0, GameManager::HEIGHT-player.h);
        player.dY = 0;
    }
    opponent.y += opponent.dY;
    if (opponent.y < 0 || opponent.y + opponent.h > GameManager::HEIGHT) {
        opponent.y = clamp(opponent.y, 0, GameManager::HEIGHT-opponent.h);
        opponent.dY = 0;
    }

    double oldX = ball.x, oldY = ball.y;
    ball.x += ball.dX;
    ball.y += ball.dY;
    if (ball.y < 0 || ball.y + ball.h > GameManager::HEIGHT) {
        if (listener != NULL)
            listener->onBounce();
        ball.dY *= -1;
        // Like with the paddle bouncing code below, extra distance is
        // re-applied, so the ball travels at a consistent rate
        // despite bouncing.
        if (ball.y < 0)
            ball.y = ball.dY - oldY;
        else
            ball.y = GameManager::HEIGHT - ball.h + ball.dY + (GameManager::HEIGHT - ball.h) - oldY;
    }

    bool which, collisionPlayer = checkCollision(ball, player), collisionOpponent = checkCollision(ball, opponent);
    // This is in case the ball's movement speed is greater than the
    // width of a paddle, which would otherwise allow the ball to
    // phase through it. It checks if the ball went completely through
    // the paddle's area, and if so "moves" the ball along its path to
    // the paddle's x axis and checks for collision there.
    if ((which = (oldX > player.x + player.w && ball.x < player.x)) || (oldX < opponent.x && ball.x > opponent.x + opponent.w)) {
        if (which) {
            double intersectY = ball.dY/ball.dX * (player.x + player.w - oldX) + ball.y;
            Entity testBall(player.x + player.w, intersectY, ball.w, ball.h, 0, 0);
            collisionPlayer |= checkCollision(testBall, player);
        } else {
            double intersectY = ball.dY/ball.dX * (opponent.x - ball.w - oldX) + ball.y;
            Entity testBall(opponent.x - ball.w, intersectY, ball.w, ball.h, 0, 0);
            collisionOpponent |= checkCollision(testBall, opponent);
        }
    }

    // The collided flag prevents the ball from colliding with the
    // same paddle multiple times in one hit; this is an issue, for
    // instance, when a paddle hits a ball with one of its smaller
    // side edges (as the ball may not move out of the way quickly enough).
    if (!collided && (collisionPlayer || collisionOpponent)) {
        if (listener != NULL)
            listener->onHit();
        ball.dX = clamp(ball.dX * -1.1, -923, 923);
        if (collisionPlayer) {
            ball.dY = clamp(ball.dY + player.dY / 2, -747, 747);
            if (ball.x + ball.w >= player.x && ball.x <= player.x + player.w) {
                collided = true;
            } else {
                // If the ball hits the paddle fast enough to go
                // through it, the extra distance (that it would have
                // gone if it hadn't hit the paddle) is reapplied in
                // the ball's new direction.
                double dXremainder = ball.dX - oldX + player.x + player.w;
                ball.x = player.x + player.w + dXremainder;
            }
        } else {
            ball.dY = clamp(ball.dY + opponent.dY / 2, -747, 747);
            if (ball.x + ball.w >= opponent.x && ball.x <= opponent.x + opponent.w) {
                collided = true;
            } else {
                double dXremainder = ball.dX + opponent.x - ball.w - oldX;
                ball.x = opponent.x - ball.w + dXremainder;
            }
        }
    } else if (collided && !collisionPlayer && !collisionOpponent) {
        collided = false;
    }

    if (player.dY > 0)
        player.dY = clamp(player.dY - .1, 0, 10);
    else
        player.dY = clamp(player.dY + .1, -10, 0);

    if (opponent.dY > 0)
        opponent.dY = clamp(opponent.dY - .1, 0, 10);
    else
        opponent.dY = clamp(opponent.dY + .1, -10, 0);

    if (ball.x + ball.w < 0 || ball.x > GameManager::WIDTH) {
        if (ball.x + ball.w < 0) {
            score2++;
            ball.dX = 2;
        } else {
            score1++;
            ball.dX = -2;
        }
        ball.dY = rand() % 2 * 4 - 2;
        ball.x = GameManager::WIDTH / 2 - ball.w / 2;
        ball.y = GameManager::HEIGHT / 2 - ball.h / 2;
    }
}
