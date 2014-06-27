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

DevConsole::DevConsole(GameManager *m) : GameState(m), commandInput(m->font16, 40, m->HEIGHT-60, m->WIDTH-40*2) {}

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
            PaddleInput *p1, *p2;

            if (elems[2] == "keyboard")
                p1 = new KeyboardInput(SDL_SCANCODE_W, SDL_SCANCODE_S);
            else if (elems[2].find("ai") == 0) {
                int difficulty = atoi(elems[2].substr(2).c_str());
                p1 = new AIInput((AIInput::Difficulty)difficulty);
            } else
                return false;

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
    commandInput.render(m->renderer);
}
