#include "AIInput.h"
#include "Entity.h"
#include "GameManager.h"
#include "utility.h"

AIInput::AIInput(Difficulty difficulty): difficulty(difficulty) {}

bool movingToward(const Entity &ball, double x) {
    return (ball.x < x && ball.getDX() > 0) || (ball.x > x && ball.getDX() < 0);
}

// Predict the ball's state when it reaches the vertical line at x (a
// paddle goal line).
Entity predict(Entity ball, double x) {
    double predictedY = ball.getSlope() * (x - ball.x) + ball.y;
    while (predictedY < 0 || predictedY + ball.h > GameManager::HEIGHT) {
        if (predictedY < 0) {
            ball.x = -ball.y / ball.getSlope() + ball.x;
            ball.y = 0;
        } else {
            ball.x = (GameManager::HEIGHT - ball.h - ball.y) / ball.getSlope() + ball.x;
            ball.y = GameManager::HEIGHT - ball.h;
        }
        ball.theta = 2*pi - ball.theta;
        predictedY = ball.getSlope() * (x - ball.x) + ball.y;
    }

    ball.x = x;
    ball.y = predictedY;
    return ball;
}

int AIInput::update(SharedState &state, int player) {
    // TODO: Update AI code for polypong.
    /*
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

    // Predict where and when the ball will intersect with the line x = paddle->x.
    double predictedY, time = (paddleX - state.ball.x) / state.ball.getDX();
    if (difficulty <= EASY) {
        predictedY = state.ball.y;
        time = 6;
    } else if (difficulty == MEDIUM) {
        predictedY = state.ball.getSlope() * (paddleX-state.ball.x) + state.ball.y + state.ball.h/2;
    } else if (difficulty >= HARD) {
        if (movingToward(state.ball, paddleX)) { 
            predictedY = predict(state.ball, paddleX).y + state.ball.h/2;
        } else if (difficulty == HARD) {
            return 0;
        } else if (difficulty >= NOPE) {
            // Predict where the ball will intersect with said line
            // after bouncing off the opponent's paddle.
            if (movingToward(state.ball, otherX)) {
                Entity predicted = predict(state.ball, otherX);
                predicted.setDelta(predicted.getDX() * -1.1, predicted.getDY() + other->getDY()/2);
                predicted = predict(predicted, paddleX);
                predictedY = predicted.y + predicted.h/2;
                // The travel time is actually a little less than this,
                // since the ball's dX increases after hitting the
                // opponent's paddle.
                time = (otherX - state.ball.x + otherX - paddleX) / state.ball.getDX();
            } else {
                // Our opponent already missed. We'll just head for the
                // middle in anticipation of a new ball.
                predictedY = GameManager::HEIGHT / 2;
                time = std::max((-state.ball.w-state.ball.x) / state.ball.getDX(), (GameManager::WIDTH-state.ball.x) / state.ball.getDX());
            }
        }
    }

    // Hehe.
    if (difficulty == INSANITY)
        time /= 100;

    double requiredDY = (predictedY - (paddle->y + paddle->h/2)) / time;
    double diff = paddle->getDY() - requiredDY;

    int change = 0;
    if (diff < -1)
        change = 1;
    else if (diff > 1)
        change = -1;

    return change;
    */
    return 0;
}
