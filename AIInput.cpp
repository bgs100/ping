#include "AIInput.h"
#include "Entity.h"
#include "GameManager.h"

AIInput::AIInput(Difficulty difficulty): difficulty(difficulty) {}

double predictX(Entity &ball, double y) {
    return ball.dX/ball.dY * (y-ball.y) + ball.x;
}

double predictY(Entity &ball, double x) {
    // Check if the ball is moving the wrong direction (away from x).
    if ((ball.x < x && ball.dX <= 0) || (ball.x > x && ball.dX >= 0))
        return -1;
    Entity ball2(ball.x, ball.y, ball.w, ball.h, ball.dX, ball.dY);
    double predictedY = ball2.dY/ball2.dX * (x - ball2.x) + ball2.y;
    while (predictedY < 0 || predictedY + ball2.h >= GameManager::HEIGHT) {
        if (predictedY < 0) {
            double predictedX = predictX(ball2, 0);
            ball2.x = predictedX;
            ball2.y = 0;
            ball2.dY *= -1;
        } else if (predictedY + ball2.h >= GameManager::HEIGHT) {
            double predictedX = predictX(ball2, GameManager::HEIGHT - ball2.h - 1);
            ball2.x = predictedX;
            ball2.y = GameManager::HEIGHT - ball.h;
            ball2.dY *= -1;
        }
        predictedY = ball2.dY/ball2.dX * (x - ball2.x) + ball2.y;
    }

    return predictedY + ball.h/2;
}

int AIInput::update(SharedState &state, int player) {
    Entity *paddle, *other;
    // These compensate for which side the paddle is on.
    int paddleX, otherX;

    if (player == 0) {
        paddle = &state.player;
        paddleX = paddle->x + paddle->w;
        other = &state.opponent;
        otherX = other->x - state.ball.w;
    } else {
        paddle = &state.opponent;
        paddleX = paddle->x - state.ball.w;
        other = &state.player;
        otherX = other->x + other->w;
    }

    // Predict where the ball will intersect with the line x = paddle->x.
    double predictedY;
    if (difficulty == MEDIUM)
        predictedY = state.ball.dY/state.ball.dX * (paddleX-state.ball.x) + state.ball.y + state.ball.h/2;
    else if (difficulty >= HARD)
        predictedY = predictY(state.ball, paddleX);

    double time = (paddleX - state.ball.x) / state.ball.dX;

    if (difficulty == EASY) {
        predictedY = state.ball.y;
        time = 6;
    }

    if (predictedY < 0) {
        if (difficulty == HARD)
            return 0;
        else if (difficulty >= NOPE) {
            // Predict where the ball will intersect with said line
            // after bouncing off the opponent's paddle.
            double hitY = predictY(state.ball, otherX);
            Entity ball(otherX, hitY, state.ball.w, state.ball.h, state.ball.dX * -1.1, state.ball.dY + other->dY/2);
            predictedY = predictY(ball, paddleX);
            time = (otherX - state.ball.x + otherX - paddleX) / state.ball.dX;
        }
    }

    // Hehe.
    if (difficulty == INSANITY)
        time /= 100;

    double requiredDY = (predictedY - (paddle->y + paddle->h/2)) / time;
    double diff = paddle->dY - requiredDY;

    int change = 0;
    if (diff < -1)
        change = 1;
    else if (diff > 1)
        change = -1;

    return change;
}
