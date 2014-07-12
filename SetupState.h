// -*- c++ -*-
#ifndef PING_SETUP_STATE_H
#define PING_SETUP_STATE_H

#include "GameState.h"
#include "PaddleInput.h"
#include "Texture.h"

class SetupState: public GameState {
public:
    enum Button { NONE, WALLS_MINUS, WALLS_PLUS, ADD_PLAYER, START, TYPE, CLOSE, DIFFICULTY, UP_KEY, DOWN_KEY };

    struct Selection {
        SetupState::Button button;
        int index;
    };

    SetupState(GameManager *m);
    ~SetupState();

    void handleEvent(SDL_Event &event);
    void render();

private:
    enum PlayerType { HUMAN, AI };

    struct Player {
        PlayerType type;
        PaddleInput *input;
        Texture texture1, texture2;
    };

    static Texture wppLabelText, humanText, aiText, startText, keysText;
    static const int SPACING = 20, BOX_W = 120, BOX_H = 100, START_BUTTON_Y = 550;

    std::vector<Player> players;
    bool waitingForKey;
    Selection selection;
    int wallsPerPlayer;
    Texture wppText;

    void updateWPPTexture();
    void updateTextures(Player &player);
    Selection getSelected(int x, int y);
};

#endif
