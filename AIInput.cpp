#include "AIInput.h"
#include "utility.h"

AIInput::AIInput(Game *game, Difficulty difficulty): game(game), difficulty(difficulty) {}

double predictX(Entity& ball, double y) {
    return ball.dX/ball.dY * (y-ball.y) + ball.x;
}

double predictY(Entity& ball, double x) {
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

int AIInput::update(Entity& paddle) {
    Entity *other;
    int paddleX = paddle.x, otherX;
    // Compensate for which side the paddle is on.
    if (&paddle == &game->player) {
        paddleX += paddle.w;
        other = &game->opponent;
        otherX = other->x - game->ball.w;
    } else {
        paddleX -= game->ball.w;
        other = &game->player;
        otherX = other->x + other->w;
    }

    // Predict where the ball will intersect with the line x = paddle.x
    double predictedY;
    if (difficulty == MEDIUM)
        predictedY = game->ball.dY/game->ball.dX * (paddleX-game->ball.x) + game->ball.y + game->ball.h/2;
    else if (difficulty >= HARD)
        predictedY = predictY(game->ball, paddleX);

    double time = (paddleX - game->ball.x) / game->ball.dX;

    if (predictedY < 0) {
        if (difficulty == HARD)
            return 0;
        else if (difficulty >= NOPE) {
            // Predict where the ball will intersect with said line
            // after bouncing off the opponent's paddle.
            double hitY = predictY(game->ball, otherX);
            Entity ball(otherX, hitY, game->ball.w, game->ball.h, -game->ball.dX + other->dY/2, game->ball.dY);
            predictedY = predictY(ball, paddleX);
            time = (otherX - game->ball.x + otherX - paddleX) / game->ball.dX;
        }
    }

    // Hehe.
    if (difficulty == INSANITY)
        time /= 100;

    double requiredDY = (predictedY - (paddle.y + paddle.h/2)) / time;
    double diff = paddle.dY - requiredDY;

    int change = 0;
    if (diff < -1)
        change = 1;
    else if (diff > 1)
        change = -1;

    return change;
}
