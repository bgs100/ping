#include "AIInput.h"
#include "Entity.h"
#include "GameManager.h"
#include "utility.h"

const char *AIInput::DIFFICULTY_STRS[] = { "Easy", "Medium", "Hard" };

AIInput::AIInput(Difficulty difficulty): difficulty(difficulty) {}

int AIInput::update(SharedState &state, int playerNum) {
    Entity &player = state.players[playerNum];
    Vector2 dir(cos(player.theta), sin(player.theta));
    Vector2 playerMid((player.getVertices()[1] + player.getVertices()[2]) / 2);
    Vector2 ballMid(state.ball.getCenter());
    double playerPos = playerMid * dir;
    double predictedPos, time;

    if (difficulty == EASY) {
        predictedPos = ballMid * dir;
        time = 6;
    } else if (difficulty == MEDIUM) {
        Vector2 ballDir(cos(state.ball.theta), sin(state.ball.theta));
        if (dir.cross(ballDir) != 0) {
            Vector2 ballPos = playerMid + dir * ((ballMid - playerMid).cross(ballDir) / dir.cross(ballDir));
            predictedPos = ballPos * dir;
            time = (ballPos - Vector2(state.ball.x, state.ball.y)).length() / state.ball.v;
        } else {
            predictedPos = playerPos;
            time = 1;
        }
    } else if (difficulty == HARD) {
        Entity ball(state.ball);
        bool found = false;
        int targetBoundary = state.playerToBoundaryIndex(playerNum);
        time = 0;

        // Simulate 50 bounces, max.
        for (int c = 0; !found && c < 50; c++) {
            predictedPos = playerPos;

            std::vector<Vector2> qs = ball.getVertices();
            Vector2 s(cos(ball.theta), sin(ball.theta));

            int minVertex = -1;
            int minBoundary = -1;
            double min = 1.0 / 0.0;
            
            for (unsigned int i = 0; i < state.boundaries.size(); i++) {
                Vector2 p = state.boundaries[(state.boundaries.size()+(int)i-1)%state.boundaries.size()];
                Vector2 r = state.boundaries[i] - p;
                if (state.boundaryToPlayerIndex(i) != -1)
                    p += 40 * state.scale * Vector2(-r.unit().y, r.unit().x);


                if (r.cross(s) == 0)
                    continue;

                for (int v = 0; v < 4; v++) {
                    double t = (qs[v] - p).cross(s) / r.cross(s);
                    double u = (qs[v] - p).cross(r) / r.cross(s);
                    if (t >= 0 && t <= 1 && u > 0 && u < min) {
                        minVertex = v;
                        minBoundary = i;
                        min = u;
                    }
                }
            }

            if (minVertex == -1)
                break;

            Vector2 r(state.boundaries[minBoundary] - state.boundaries[(state.boundaries.size()+minBoundary-1)%state.boundaries.size()]);
            
            time += min / ball.v;
            Vector2 bounce = qs[minVertex] + min * s;
            ball.setCenter(ball.getCenter() - qs[minVertex] + bounce);
            ball.theta = 2*atan2(r.y, r.x) - ball.theta;
            
            if (minBoundary == targetBoundary) {
                found = true;
                predictedPos = ball.getCenter() * dir;
                break;
            }
        }
    }

    double requiredV = (predictedPos - playerPos) / time;
    double diff = requiredV - player.v;

    int change = 0;
    if (diff < -1)
        change = -1;
    else if (diff > 1)
        change = 1;

    return change;

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
}
