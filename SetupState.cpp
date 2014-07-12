#include <sstream>
#include "GameManager.h"
#include "KeyboardInput.h"
#include "AIInput.h"
#include "SetupState.h"
#include "utility.h"

Texture SetupState::humanText, SetupState::aiText, SetupState::startText, SetupState::keysText;

SetupState::SetupState(GameManager *m) : GameState(m), waitingForKey(false), selection({ NONE, 0 }) {
    players.push_back({ HUMAN, new KeyboardInput(SDL_SCANCODE_W, SDL_SCANCODE_S) });
    players.push_back({ AI, new AIInput(AIInput::MEDIUM) });

    if (humanText.empty()) {
        humanText = Texture::fromText(m->renderer, m->fonts[FONT_RND][SIZE_12], "Human", 0xff, 0xff, 0xff);
        aiText = Texture::fromText(m->renderer, m->fonts[FONT_RND][SIZE_12], "AI", 0xff, 0xff, 0xff);
        startText = Texture::fromText(m->renderer, m->fonts[FONT_SQR][SIZE_48], "Start Game", 0xff, 0xff, 0xff);
        keysText = Texture::fromText(m->renderer, m->fonts[FONT_RND][SIZE_12], "Keys:", 0xff, 0xff, 0xff);
    }
}

SetupState::~SetupState() {
    for (Player &p : players)
        delete p.input;
}

// TODO: Reduce duplication between this and render() - gah!
SetupState::Selection SetupState::getSelected(int x, int y) {
    int len = (BOX_W + SPACING) * players.size() - SPACING;
    int start = (m->WIDTH - len) / 2;

    // Check for plus sign selection.
    int plusX = start + len + (len > 0 ? 20 : 0);
    if (x >= plusX && x < plusX + 40 && y >= (m->HEIGHT - 40) / 2 && y < (m->HEIGHT + 40) / 2)
        return { PLUS, 0 };

    // And for start button selection.
    if (x >= (m->WIDTH - startText.w) / 2 && x < (m->WIDTH + startText.w) / 2 && y >= START_BUTTON_Y && y < START_BUTTON_Y + startText.h)
        return { START, 0 };

    if (x >= start + len || (x - start) % (BOX_W + SPACING) >= BOX_W)
        return { NONE, 0 };

    int selectedBox = (x - start) / (BOX_W + SPACING);
    int interiorX = x - (start + selectedBox * (BOX_W + SPACING));
    int interiorY = y - (m->HEIGHT - BOX_H) / 2;

    // Check for selection of the opposite button.
    SDL_Rect rect = { 15, 38, humanText.w + 10, humanText.h + 3 };
    if (players[selectedBox].type == HUMAN)
        rect = { BOX_W - aiText.w - 25, 38, aiText.w + 10, aiText.h + 3 };
    
    if (interiorX >= rect.x && interiorX < rect.x + rect.w && interiorY >= rect.y && interiorY < rect.y + rect.h)
        return { TYPE, selectedBox };

    // Check for selection of the close button.
    rect = { BOX_W - 15, 5, 10, 10 };
    if (interiorX >= rect.x && interiorX < rect.x + rect.w && interiorY >= rect.y && interiorY < rect.y + rect.h)
        return { CLOSE, selectedBox };

    // Check for difficulty selection.
    int w, h;
    if (players[selectedBox].type == AI) {
        TTF_SizeText(m->fonts[FONT_RND][SIZE_12], AIInput::DIFFICULTY_STRS[((AIInput *)players[selectedBox].input)->difficulty], &w, &h);
        rect = { (BOX_W - w) / 2, BOX_H - h - 10, w, h };
        if (interiorX >= rect.x && interiorX < rect.x + rect.w && interiorY >= rect.y && interiorY < rect.y + rect.h)
            return { DIFFICULTY, selectedBox };
        return { NONE, 0 };
    }

    // Check for key selection.
    TTF_SizeText(m->fonts[FONT_RND][SIZE_12],
                 getShortKeyName(SDL_GetKeyFromScancode(((KeyboardInput *)players[selectedBox].input)->upKey), 7, 6), &w, &h);
    rect = { 65 + (BOX_W - 75 - w)/2, BOX_H - 2 * keysText.h + 1, w, h };
    if (interiorX >= rect.x && interiorX < rect.x + rect.w && interiorY >= rect.y && interiorY < rect.y + rect.h)
        return { UP_KEY, selectedBox };

    TTF_SizeText(m->fonts[FONT_RND][SIZE_12],
                 getShortKeyName(SDL_GetKeyFromScancode(((KeyboardInput *)players[selectedBox].input)->downKey), 7, 6), &w, &h);
    rect = { 65 + (BOX_W - 75 - w)/2, BOX_H - keysText.h - 2, w, h };
    if (interiorX >= rect.x && interiorX < rect.x + rect.w && interiorY >= rect.y && interiorY < rect.y + rect.h)
        return { DOWN_KEY, selectedBox };

    return { NONE, 0 };
}

void SetupState::handleEvent(SDL_Event &event) {
    if (event.type == SDL_MOUSEMOTION && !waitingForKey) {
        selection = getSelected(event.motion.x, event.motion.y);
    } else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT && !waitingForKey) {
        Selection selected = getSelected(event.button.x, event.button.y);
        bool reselect = true;
        if (selected.button == PLUS)
            players.push_back({ AI, new AIInput(AIInput::MEDIUM) });
        else if (selected.button == START && players.size() > 1) {
            std::vector<PaddleInput *> inputs(players.size());
            for (unsigned int i = 0; i < players.size(); i++) {
                inputs[i] = players[i].input;
                if (players[i].type == HUMAN)
                    players[i].input = new KeyboardInput(*((KeyboardInput *)inputs[i]));
                else
                    players[i].input = new AIInput(*((AIInput *)inputs[i]));
            }
            m->pushState(new Game(m, inputs, inputs.size() > 2 ? 1 : 2));
            reselect = false;
        } else if (selected.button == TYPE) {
            delete players[selected.index].input;
            
            if (players[selected.index].type == HUMAN) {
                players[selected.index].type = AI;
                players[selected.index].input = new AIInput(AIInput::MEDIUM);
            } else {
                players[selected.index].type = HUMAN;
                players[selected.index].input = new KeyboardInput(SDL_SCANCODE_W, SDL_SCANCODE_S);
            }
        } else if (selected.button == CLOSE) {
            delete players[selected.index].input;
            players.erase(players.begin() + selected.index);
        } else if (selected.button == DIFFICULTY) {
            AIInput &input = *((AIInput *)players[selected.index].input);
            input.difficulty = (AIInput::Difficulty)((input.difficulty + 1) % AIInput::NUM_DIFFICULTY);
        } else if (selected.button == UP_KEY || selected.button == DOWN_KEY) {
            waitingForKey = true;
        }

        // TODO: Implement better solution here; if the player exits
        // the game and the cursor happens to land on the start
        // button, it won't light up until the player moves the mouse,
        // which, while better than the start button staying lit up
        // even when the cursor *isn't* on it, is still worse than
        // just having correct behavior (which would require some sort
        // of resume() or reenter() method for GameStates).
        if (reselect)
            selection = getSelected(event.button.x, event.button.y);
        else
            selection.button = NONE;
    } else if (event.type == SDL_KEYDOWN && waitingForKey) {
        if (selection.button == UP_KEY)
            ((KeyboardInput *)players[selection.index].input)->upKey = event.key.keysym.scancode;
        else
            ((KeyboardInput *)players[selection.index].input)->downKey = event.key.keysym.scancode;
        waitingForKey = false;

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

        SDL_Rect rect = { x, y, BOX_W, BOX_H };
        SDL_RenderDrawRect(m->renderer, &rect);

        // Draw a close button (which removes the player).
        rect = { x + BOX_W - 15, y + 5, 10, 10 };
        if (selection.button != CLOSE || (int)i != selection.index)
            SDL_SetRenderDrawColor(m->renderer, 0xaa, 0xaa, 0xaa, 0xaa);

        SDL_RenderDrawRect(m->renderer, &rect);
        SDL_RenderDrawLine(m->renderer, x + BOX_W - 15, y + 5, x + BOX_W - 7, y + 13);
        SDL_RenderDrawLine(m->renderer, x + BOX_W - 14, y + 13, x + BOX_W - 7, y + 6);

        SDL_SetRenderDrawColor(m->renderer, 0xff, 0xff, 0xff, 0xff);

        // Draw "Player X" at the top of the box.
        std::stringstream ss;
        ss << "Player " << (i + 1);
        Texture text(Texture::fromText(m->renderer, m->fonts[FONT_RND][SIZE_16], ss.str().c_str(), 0xff, 0xff, 0xff));
        text.render(m->renderer, x + (BOX_W - text.w) / 2, y + 8);

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
        
        SDL_SetRenderDrawColor(m->renderer, 0xaa, 0xaa, 0xaa, 0xaa);
        SDL_RenderDrawRect(m->renderer, unselectedRect);

        // TODO: Maybe move this into Player and only recreate the
        // textures when necessary?
        if (players[i].type == HUMAN) {
            const char *upText = "_", *downText = "_";

            if (!waitingForKey || selection.button != UP_KEY || (int)i != selection.index)
                upText = getShortKeyName(SDL_GetKeyFromScancode(((KeyboardInput *)players[i].input)->upKey), 7, 6);

            Texture keyUp(Texture::fromText(m->renderer, m->fonts[FONT_RND][SIZE_12], upText, 0xff, 0xff, 0xff));

            if (!waitingForKey || selection.button != DOWN_KEY || (int)i != selection.index)
                downText = getShortKeyName(SDL_GetKeyFromScancode(((KeyboardInput *)players[i].input)->downKey), 7, 6);

            Texture keyDown(Texture::fromText(m->renderer, m->fonts[FONT_RND][SIZE_12], downText, 0xff, 0xff, 0xff));

            if (selection.button != UP_KEY || (int)i != selection.index)
                keyUp.setAlphaMod(0xbb);
            if (selection.button != DOWN_KEY || (int)i != selection.index)
                keyDown.setAlphaMod(0xbb);

            keysText.render(m->renderer, x + 15, y + BOX_H - 3 * keysText.h / 2 - 1);
            keyUp.render(m->renderer, x + 65 + (BOX_W - 75 - keyUp.w)/2, y + BOX_H - 2 * keysText.h + 1);
            keyDown.render(m->renderer, x + 65 + (BOX_W - 75 - keyDown.w)/2, y + BOX_H - keysText.h - 2);
        } else if (players[i].type == AI) {
            Texture difficulty(Texture::fromText(m->renderer, m->fonts[FONT_RND][SIZE_12],
                                                 AIInput::DIFFICULTY_STRS[((AIInput *)players[i].input)->difficulty], 0xff, 0xff, 0xff));
            if (selection.button != DIFFICULTY || (int)i != selection.index)
                difficulty.setAlphaMod(0xbb);
            difficulty.render(m->renderer, x + (BOX_W - difficulty.w)/2, y + BOX_H - difficulty.h - 10);
        }
    }

    if (selection.button == PLUS)
        SDL_SetRenderDrawColor(m->renderer, 0xff, 0xff, 0xff, 0xff);
    else
        SDL_SetRenderDrawColor(m->renderer, 0xbb, 0xbb, 0xbb, 0xbb);

    // Draw the plus sign.
    int plusX = start + len + (len > 0 ? 20 : 0);
    SDL_Rect rect = { plusX, (m->HEIGHT - 10) / 2, 40, 10 };
    SDL_RenderFillRect(m->renderer, &rect);
    rect = { plusX + 15, (m->HEIGHT - 40) / 2, 10, 40 };
    SDL_RenderFillRect(m->renderer, &rect);

    if (players.size() < 2)
        startText.setAlphaMod(0x55);
    else if (selection.button == START)
        startText.setAlphaMod(0xff);
    else
        startText.setAlphaMod(0xbb);

    startText.render(m->renderer, (m->WIDTH - startText.w) / 2, START_BUTTON_Y);
}
