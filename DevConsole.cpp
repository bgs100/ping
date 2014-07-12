#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
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

DevConsole::DevConsole(GameManager *m) : GameState(m), commandInput(m->fonts[FONT_SQR][SIZE_16], 40, m->HEIGHT-60, m->WIDTH-40*2) {}

void DevConsole::handleEvent(SDL_Event &event) {
    if (commandInput.handleEvent(event)) {
        if (handleCommand(commandInput.text))
            commandInput.clear();
    }
}

bool DevConsole::handleCommand(std::string input) {
    std::vector<std::string> elems;
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
    split(input, ' ', elems);
    if (elems.size() < 1)
        return false;
    if (elems[0] == "push" && elems.size() >= 2) {
        if (elems[1] == "game" && elems.size() >= 4) {
            std::vector<PaddleInput *> inputs(2);

            if (elems[2] == "keyboard")
                inputs[0] = new KeyboardInput(SDL_SCANCODE_W, SDL_SCANCODE_S);
            else if (elems[2].find("ai") == 0) {
                int difficulty = atoi(elems[2].substr(2).c_str());
                inputs[0] = new AIInput((AIInput::Difficulty)difficulty);
            } else
                return false;

            const char *host = NULL;
            if (elems[3] == "keyboard")
                inputs[1] = new KeyboardInput(SDL_SCANCODE_UP, SDL_SCANCODE_DOWN);
            else if (elems[3].find("ai") == 0) {
                int difficulty = atoi(elems[3].substr(2).c_str());
                inputs[1] = new AIInput((AIInput::Difficulty)difficulty);
            } else if (elems[3] == "null" && elems.size() >= 5) {
                host = elems[4].c_str();
                m->pushState(new Game(m, inputs[0], host));
            } else {
                host = elems[3].c_str();
                m->pushState(new Game(m, inputs[0], host));
            }

            if (host == NULL)
                m->pushState(new Game(m, inputs, 2));

            return true;
        }
    }

    return false;
}

void DevConsole::render() {
    commandInput.render(m->renderer);
}
