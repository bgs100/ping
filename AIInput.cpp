#include "AIInput.h"
#include "utility.h"

AIInput::AIInput(Game *game): game(game) {}

int AIInput::update(Entity& paddle) {
    int ballX = game->ball.x;
    int paddleX = paddle.x;
    // Compensate for which side the paddle is on.
    if (&paddle == &game->player)
        paddleX += paddle.w;
    else
        ballX += game->ball.w;

    // Predict where the ball will intersect with the line x = paddle.x
    double predictedY = game->ball.dY/game->ball.dX * (paddleX-ballX) + game->ball.y + game->ball.h/2;
    double time = (paddleX - ballX) / game->ball.dX;
    double requiredDY = (predictedY - (paddle.y + paddle.h/2)) / time;
    double diff = paddle.dY - requiredDY;

    int change = 0;
    if (diff < -1)
        change = 1;
    else if (diff > 1)
        change = -1;

    return change;
}
