#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string.h>
#include <stdlib.h>
#include "DevConsole.h"
#include "GameManager.h"
#include "Game.h"
#include "KeyboardInput.h"
#include "AIInput.h"

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

DevConsole::DevConsole(GameManager *m) : GameState(m), inputTexture(NULL) {
    inputText[0] = '\0';
    SDL_StartTextInput();
}

DevConsole::~DevConsole() {
    delete inputTexture;
}

void DevConsole::handleEvent(SDL_Event &event) {
    bool renderText = false;

    // TODO: Reduce code duplication with MultiplayerMenu.
    if (event.type == SDL_KEYDOWN) {
        int len = strlen(inputText);
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_BACKSPACE && len > 0) {
            inputText[len-1] = '\0';
            renderText = true;
        } else if (SDL_GetModState() & KMOD_CTRL && key == SDLK_c) {
            SDL_SetClipboardText(inputText);
        } else if (SDL_GetModState() & KMOD_CTRL && key == SDLK_v) {
            strncpy(inputText, SDL_GetClipboardText(), 260);
            inputText[259] = '\0';
            renderText = true;
        } else if (key == SDLK_RETURN) {
            if (handleCommand()) {
                inputText[0] = '\0';
                renderText = true;
            }
        } else if (key == SDLK_ESCAPE) {
            m->revertState();
        }
    } else if (event.type == SDL_TEXTINPUT) {
        char *text = event.text.text;
        if (!(SDL_GetModState() & KMOD_CTRL && (tolower(text[0]) == 'c' || tolower(text[0]) == 'v'))) {
            strncat(inputText, text, 260-strlen(inputText));
            renderText = true;
        }
    }

    if (renderText) {
        delete inputTexture;
        inputTexture = Texture::fromText(m->renderer, m->font16, inputText, 0xff, 0xff, 0xff);
    }
}

bool DevConsole::handleCommand() {
    std::vector<std::string> elems;
    std::string input(inputText);
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
    split(input, ' ', elems);
    if (elems.size() < 1)
        return false;
    if (elems[0] == "push" && elems.size() >= 2) {
        if (elems[1] == "game" && elems.size() >= 4) {
            PaddleInput *p1, *p2;

            if (elems[2] == "keyboard")
                p1 = new KeyboardInput(SDL_SCANCODE_W, SDL_SCANCODE_S);
            else if (elems[2].find("ai") == 0) {
                int difficulty = atoi(elems[2].substr(2).c_str());
                p1 = new AIInput((AIInput::Difficulty)difficulty);
            } else {
                m->revertState();
                return false;
            }

            const char *host = NULL;
            if (elems[3] == "keyboard")
                p2 = new KeyboardInput(SDL_SCANCODE_UP, SDL_SCANCODE_DOWN);
            else if (elems[3].find("ai") == 0) {
                int difficulty = atoi(elems[3].substr(2).c_str());
                p2 = new AIInput((AIInput::Difficulty)difficulty);
            } else if (elems[3] == "null" && elems.size() >= 5) {
                host = elems[4].c_str();
                m->pushState(new Game(m, p1, host));
            } else {
                host = elems[3].c_str();
                m->pushState(new Game(m, p1, host));
            }

            if (host == NULL)
                m->pushState(new Game(m, p1, p2));

            return true;
        }
    }

    return false;
}

void DevConsole::render() {
    SDL_SetRenderDrawColor(m->renderer, 0xaa, 0xaa, 0xaa, 0xff);
    SDL_Rect rect = { 40, m->HEIGHT-60, m->WIDTH-80, TTF_FontHeight(m->font24) };
    SDL_RenderDrawRect(m->renderer, &rect);

    if (inputTexture != NULL)
        inputTexture->render(m->renderer, 50, m->HEIGHT-60);
}
