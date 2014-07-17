#include <sstream>
#include "GameManager.h"
#include "KeyboardInput.h"
#include "AIInput.h"
#include "SetupState.h"
#include "utility.h"

Texture SetupState::wppLabelText, SetupState::humanText, SetupState::aiText, SetupState::startText, SetupState::keysText;

// classic is false by default (see SetupState.h).
SetupState::SetupState(GameManager *m, bool classic)
    : GameState(m), waitingForKey(false), classic(classic), selection({ NONE, 0 }), wallsPerPlayer(2) {
    players.push_back({ HUMAN, new KeyboardInput(SDL_SCANCODE_W, SDL_SCANCODE_S) });
    players.push_back({ AI, new AIInput(AIInput::MEDIUM) });

    updateTextures(players[0]);
    updateTextures(players[1]);

    updateWPPTexture();

    if (wppLabelText.empty()) {
        wppLabelText = Texture::fromText(m->renderer, m->fonts[FONT_RND][SIZE_24], "Walls Per Player:");
        humanText = Texture::fromText(m->renderer, m->fonts[FONT_RND][SIZE_12], "Human");
        aiText = Texture::fromText(m->renderer, m->fonts[FONT_RND][SIZE_12], "AI");
        startText = Texture::fromText(m->renderer, m->fonts[FONT_SQR][SIZE_48], "Start Game");
        keysText = Texture::fromText(m->renderer, m->fonts[FONT_RND][SIZE_12], "Keys:");
    }
}

SetupState::~SetupState() {
    for (Player &p : players)
        delete p.input;
}

void SetupState::updateWPPTexture() {
    char buf[21];
    wppText = Texture::fromText(m->renderer, m->fonts[FONT_SQR][SIZE_32], itoa(wallsPerPlayer, buf, 21));
}

void SetupState::updateTextures(Player &player) {
    if (player.type == HUMAN) {
        const char *upText = "_", *downText = "_";

        if (!waitingForKey || selection.button != UP_KEY)
            upText = getShortKeyName(SDL_GetKeyFromScancode(((KeyboardInput *)player.input)->upKey), 7, 6);
        player.texture1 = Texture::fromText(m->renderer, m->fonts[FONT_RND][SIZE_12], upText);

        if (!waitingForKey || selection.button != DOWN_KEY)
            downText = getShortKeyName(SDL_GetKeyFromScancode(((KeyboardInput *)player.input)->downKey), 7, 6);
        player.texture2 = Texture::fromText(m->renderer, m->fonts[FONT_RND][SIZE_12], downText);
    } else {
        player.texture1 = Texture::fromText(m->renderer, m->fonts[FONT_RND][SIZE_12],
                                            AIInput::DIFFICULTY_STRS[((AIInput *)player.input)->difficulty]);
    }
}

inline bool pointInRect(int x, int y, const SDL_Rect &rect) {
    return (x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h);
}

SetupState::Selection SetupState::getSelected(int x, int y) {
    int len = (BOX_W + SPACING) * players.size() - SPACING;
    int start = (m->WIDTH - len) / 2;

    // Check for walls per player decrement button selection.
    if (pointInRect(x, y, { (m->WIDTH - wppText.w - 26)/2 - 30 - 12, 158 + (wppText.h + 4 - 8) / 2, 26, 8 }))
        return { WALLS_MINUS, 0 };

    // Check for walls per player incremenet button selection.
    int plusX = (m->WIDTH + wppText.w) / 2 + 27;
    if (pointInRect(x, y, { plusX, 158 + (wppText.h + 4 - 8) / 2, 34, 8 }) ||
        pointInRect(x, y, { plusX + 13, 158 + (wppText.h + 4 - 34) / 2, 8, 34 }))
        return { WALLS_PLUS, 0 };

    // Check for add player (plus) button selection.
    plusX = start + len + (len > 0 ? 20 : 0);
    if (!classic && (pointInRect(x, y, { plusX, (m->HEIGHT - 10) / 2, 40, 10 }) ||
                     pointInRect(x, y, { plusX + 15, (m->HEIGHT - 40) / 2, 10, 40 })))
        return { ADD_PLAYER, 0 };

    // And for start button selection.
    if (pointInRect(x, y, { (m->WIDTH - startText.w) / 2, START_BUTTON_Y, startText.w, startText.h }))
        return { START, 0 };

    if (x < start || x >= start + len || (x - start) % (BOX_W + SPACING) >= BOX_W)
        return { NONE, 0 };

    int selectedBox = (x - start) / (BOX_W + SPACING);
    int interiorX = x - (start + selectedBox * (BOX_W + SPACING));
    int interiorY = y - (m->HEIGHT - BOX_H) / 2;

    // Check for selection of the opposite button.
    SDL_Rect rect = { 15, 38, humanText.w + 10, humanText.h + 3 };
    if (players[selectedBox].type == HUMAN)
        rect = { BOX_W - aiText.w - 25, 38, aiText.w + 10, aiText.h + 3 };
    
    if (pointInRect(interiorX, interiorY, rect))
        return { TYPE, selectedBox };

    // Check for selection of the close button.
    if (!classic && pointInRect(interiorX, interiorY, { BOX_W - 15, 5, 10, 10 }))
        return { CLOSE, selectedBox };

    // Check for difficulty selection.
    int w, h;
    if (players[selectedBox].type == AI) {
        w = players[selectedBox].texture1.w;
        h = players[selectedBox].texture1.h;
        if (pointInRect(interiorX, interiorY, { (BOX_W - w) / 2, BOX_H - h - 10, w, h }))
            return { DIFFICULTY, selectedBox };
        return { NONE, 0 };
    }

    // Check for key selection.
    w = players[selectedBox].texture1.w;
    h = players[selectedBox].texture1.h;
    if (pointInRect(interiorX, interiorY, { 65 + (BOX_W - 75 - w)/2, BOX_H - 2 * keysText.h + 1, w, h }))
        return { UP_KEY, selectedBox };

    w = players[selectedBox].texture2.w;
    h = players[selectedBox].texture2.h;
    if (pointInRect(interiorX, interiorY, { 65 + (BOX_W - 75 - w)/2, BOX_H - keysText.h - 2, w, h }))
        return { DOWN_KEY, selectedBox };

    return { NONE, 0 };
}

void SetupState::handleEvent(SDL_Event &event) {
    if (event.type == SDL_MOUSEMOTION && !waitingForKey) {
        selection = getSelected(event.motion.x, event.motion.y);
    } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT && !waitingForKey) {
        selection = getSelected(event.button.x, event.button.y);
        bool reselect = true;
        if (selection.button == WALLS_MINUS && wallsPerPlayer > (players.size() > 2 ? 1 : 2)) {
            wallsPerPlayer--;
            updateWPPTexture();
        } else if (selection.button == WALLS_PLUS) {
            wallsPerPlayer++;
            updateWPPTexture();
        } else if (selection.button == ADD_PLAYER) {
            players.push_back({ AI, new AIInput(AIInput::MEDIUM) });
            updateTextures(*(players.end()-1));
        } else if (selection.button == START && players.size() > 1) {
            std::vector<PaddleInput *> inputs(players.size());
            for (unsigned int i = 0; i < players.size(); i++) {
                inputs[i] = players[i].input;
                // Copy the inputs, since Game::~Game() necessarily deletes them.
                if (players[i].type == HUMAN)
                    players[i].input = new KeyboardInput(*((KeyboardInput *)inputs[i]));
                else
                    players[i].input = new AIInput(*((AIInput *)inputs[i]));
            }
            m->pushState(new Game(m, inputs, wallsPerPlayer, classic));
            reselect = false;
        } else if (selection.button == TYPE) {
            delete players[selection.index].input;
            
            if (players[selection.index].type == HUMAN) {
                players[selection.index].type = AI;
                players[selection.index].input = new AIInput(AIInput::MEDIUM);
            } else {
                players[selection.index].type = HUMAN;
                players[selection.index].input = new KeyboardInput(SDL_SCANCODE_W, SDL_SCANCODE_S);
            }

            updateTextures(players[selection.index]);
        } else if (selection.button == CLOSE) {
            delete players[selection.index].input;
            players.erase(players.begin() + selection.index);
            if (players.size() == 2 && wallsPerPlayer < 2) {
                wallsPerPlayer = 2;
                updateWPPTexture();
            }
        } else if (selection.button == DIFFICULTY) {
            AIInput &input = *((AIInput *)players[selection.index].input);
            input.difficulty = (AIInput::Difficulty)((input.difficulty + 1) % AIInput::NUM_DIFFICULTY);
            updateTextures(players[selection.index]);
        } else if (selection.button == UP_KEY || selection.button == DOWN_KEY) {
            waitingForKey = true;
            updateTextures(players[selection.index]);
        }

        // TODO: Implement better solution here; if the player exits
        // the game and the cursor happens to land on the start
        // button, it won't light up until the player moves the mouse,
        // which, while better than the start button staying lit up
        // even when the cursor *isn't* on it, is still worse than
        // just having correct behavior (which would require some sort
        // of resume() or reenter() method for GameStates).
        if (reselect && !waitingForKey)
            selection = getSelected(event.button.x, event.button.y);
        else if (!waitingForKey)
            selection.button = NONE;
    } else if (event.type == SDL_KEYDOWN && waitingForKey) {
        if (selection.button == UP_KEY)
            ((KeyboardInput *)players[selection.index].input)->upKey = event.key.keysym.scancode;
        else
            ((KeyboardInput *)players[selection.index].input)->downKey = event.key.keysym.scancode;

        waitingForKey = false;
        updateTextures(players[selection.index]);

        int x, y;
        SDL_GetMouseState(&x, &y);
        selection = getSelected(x, y);
    }
}

void SetupState::render() {
    int len = (BOX_W + SPACING) * players.size() - SPACING;
    int start = (m->WIDTH - len) / 2;
    for (unsigned int i = 0; i < players.size(); i++) {
        int x = start + i * (BOX_W + SPACING);
        int y = (m->HEIGHT - BOX_H) / 2;

        SDL_SetRenderDrawColor(m->renderer, 0xff, 0xff, 0xff, 0xff);

        // Outline the whole box.
        SDL_Rect rect = { x, y, BOX_W, BOX_H };
        SDL_RenderDrawRect(m->renderer, &rect);

        // Draw a close button (which removes the player).
        if (!classic) {
            rect = { x + BOX_W - 15, y + 5, 10, 10 };
            if (selection.button != CLOSE || (int)i != selection.index)
                SDL_SetRenderDrawColor(m->renderer, 0xaa, 0xaa, 0xaa, 0xff);
            
            SDL_RenderDrawRect(m->renderer, &rect);
            SDL_RenderDrawLine(m->renderer, x + BOX_W - 15, y + 5, x + BOX_W - 7, y + 13);
            SDL_RenderDrawLine(m->renderer, x + BOX_W - 14, y + 13, x + BOX_W - 7, y + 6);
   
            SDL_SetRenderDrawColor(m->renderer, 0xff, 0xff, 0xff, 0xff);
        }

        // Draw "Player X" at the top of the box.
        std::stringstream ss;
        ss << "Player " << (i + 1);
        Texture text(Texture::fromText(m->renderer, m->fonts[FONT_RND][SIZE_16], ss.str().c_str()));
        text.render(m->renderer, x + (BOX_W - text.w) / 2, y + 8);

        // Draw human/AI toggle boxes.
        int humanX = x + 20, aiX = x + BOX_W - aiText.w - 20;

        SDL_Rect humanRect = { humanX - 5, y + 38, humanText.w + 10, humanText.h + 3 };
        SDL_Rect aiRect = { aiX - 5, y + 38, aiText.w + 10, aiText.h + 3 };

        SDL_Rect *selectedRect = &humanRect, *unselectedRect = &aiRect;

        if (players[i].type == HUMAN) {
            aiText.setAlphaMod(0xaa);
        } else {
            humanText.setAlphaMod(0xaa);
            selectedRect = &aiRect;
            unselectedRect = &humanRect;
        }

        if (selection.button == TYPE && (int)i == selection.index) {
            humanText.setAlphaMod(0xff);
            aiText.setAlphaMod(0xff);
        }

        humanText.render(m->renderer, humanX, y + 40);
        aiText.render(m->renderer, aiX, y + 40);

        humanText.setAlphaMod(0xff);
        aiText.setAlphaMod(0xff);
        
        SDL_RenderDrawRect(m->renderer, selectedRect);
        
        SDL_SetRenderDrawColor(m->renderer, 0xaa, 0xaa, 0xaa, 0xff);
        SDL_RenderDrawRect(m->renderer, unselectedRect);

        // Draw human/AI specific stuff (difficulty, keybindings).
        if (players[i].type == HUMAN) {
            if (selection.button != UP_KEY || (int)i != selection.index)
                players[i].texture1.setAlphaMod(0xbb);
            if (selection.button != DOWN_KEY || (int)i != selection.index)
                players[i].texture2.setAlphaMod(0xbb);

            keysText.render(m->renderer, x + 15, y + BOX_H - 3 * keysText.h / 2 - 1);
            players[i].texture1.render(m->renderer, x + 65 + (BOX_W - 75 - players[i].texture1.w)/2, y + BOX_H - 2 * keysText.h + 1);
            players[i].texture2.render(m->renderer, x + 65 + (BOX_W - 75 - players[i].texture2.w)/2, y + BOX_H - keysText.h - 2);

            players[i].texture1.setAlphaMod(0xff);
            players[i].texture2.setAlphaMod(0xff);
        } else if (players[i].type == AI) {
            if (selection.button != DIFFICULTY || (int)i != selection.index)
                players[i].texture1.setAlphaMod(0xbb);

            players[i].texture1.render(m->renderer, x + (BOX_W - players[i].texture1.w)/2, y + BOX_H - players[i].texture1.h - 10);
            players[i].texture1.setAlphaMod(0xff);
        }
    }

    // Draw the start button.
    if (players.size() < 2)
        startText.setAlphaMod(0x55);
    else if (selection.button == START)
        startText.setAlphaMod(0xff);
    else
        startText.setAlphaMod(0xbb);

    startText.render(m->renderer, (m->WIDTH - startText.w) / 2, START_BUTTON_Y);

    if (classic)
        return;

    SDL_SetRenderDrawColor(m->renderer, 0xff, 0xff, 0xff, 0xff);

    // Draw walls per player stuff.
    wppLabelText.render(m->renderer, (m->WIDTH - wppLabelText.w)/2, 115);
    wppText.render(m->renderer, (m->WIDTH - wppText.w)/2, 160);

    SDL_Rect rect = { (m->WIDTH - wppText.w - 30)/2, 158, wppText.w + 30, wppText.h + 4 };
    SDL_RenderDrawRect(m->renderer, &rect);

    if (wallsPerPlayer <= (players.size() > 2 ? 1 : 2))
        SDL_SetRenderDrawColor(m->renderer, 0x55, 0x55, 0x55, 0xff);
    else if (selection.button == WALLS_MINUS)
        SDL_SetRenderDrawColor(m->renderer, 0xff, 0xff, 0xff, 0xff);
    else
        SDL_SetRenderDrawColor(m->renderer, 0xbb, 0xbb, 0xbb, 0xff);

    rect = { (m->WIDTH - wppText.w - 26)/2 - 30 - 12, 158 + (wppText.h + 4 - 8) / 2, 26, 8 };
    SDL_RenderFillRect(m->renderer, &rect);

    if (selection.button == WALLS_PLUS)
        SDL_SetRenderDrawColor(m->renderer, 0xff, 0xff, 0xff, 0xff);
    else
        SDL_SetRenderDrawColor(m->renderer, 0xbb, 0xbb, 0xbb, 0xff);

    int plusX = (m->WIDTH + wppText.w) / 2 + 27;
    rect = { plusX, 158 + (wppText.h + 4 - 8) / 2, 34, 8 };
    SDL_RenderFillRect(m->renderer, &rect);
    rect = { plusX + 13, 158 + (wppText.h + 4 - 34) / 2, 8, 34 };
    SDL_RenderFillRect(m->renderer, &rect);

    // Draw the button to add players (a plus sign).
    if (selection.button == ADD_PLAYER)
        SDL_SetRenderDrawColor(m->renderer, 0xff, 0xff, 0xff, 0xff);
    else
        SDL_SetRenderDrawColor(m->renderer, 0xbb, 0xbb, 0xbb, 0xff);

    plusX = start + len + (len > 0 ? 20 : 0);
    rect = { plusX, (m->HEIGHT - 10) / 2, 40, 10 };
    SDL_RenderFillRect(m->renderer, &rect);
    rect = { plusX + 15, (m->HEIGHT - 40) / 2, 10, 40 };
    SDL_RenderFillRect(m->renderer, &rect);
}
