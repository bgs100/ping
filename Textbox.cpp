#include "Textbox.h"

#include <iostream>

Textbox::Textbox(TTF_Font *font, int x, int y, int w)
    : font(font), x(x), y(y), w(w), h(TTF_FontHeight(font)), texture(NULL), renderText(false), pos(0) {
    SDL_StartTextInput();
}

Textbox::~Textbox() {
    SDL_StopTextInput();
    delete texture;
}

void Textbox::clear() {
    text.erase();
    renderText = true;
}

bool Textbox::handleEvent(SDL_Event &event) {
    if (event.type == SDL_KEYDOWN) {
        SDL_Keycode key = event.key.keysym.sym;
        if (key == SDLK_BACKSPACE && pos > 0) {
            text.erase(text.begin() + --pos);
            renderText = true;
        } else if (key == SDLK_DELETE && pos < text.size()) {
            text.erase(text.begin() + pos);
            renderText = true;
        } else if (SDL_GetModState() & KMOD_CTRL && key == SDLK_c) {
            SDL_SetClipboardText(text.c_str());
        } else if (SDL_GetModState() & KMOD_CTRL && key == SDLK_v) {
            int size = text.size();
            text.insert(pos, SDL_GetClipboardText());
            pos += text.size() - size;
            renderText = true;
        } else if (key == SDLK_LEFT && pos > 0) {
            pos--;
        } else if (key == SDLK_RIGHT && pos < text.size()) {
            pos++;
        } else if (key == SDLK_HOME) {
            pos = 0;
        } else if (key == SDLK_END) {
            pos = text.size();
        } else if (key == SDLK_RETURN) {
            return true;
        }
    } else if (event.type == SDL_TEXTINPUT) {
        char *input = event.text.text;
        if (!(SDL_GetModState() & KMOD_CTRL && (tolower(input[0]) == 'c' || tolower(input[0]) == 'v'))) {
            int size = text.size();
            text.insert(pos, input);
            pos += text.size() - size;
            renderText = true;
        }
    }

    return false;
}

void Textbox::render(SDL_Renderer *renderer) {
    if (renderText) {
        renderText = false;
        delete texture;
        texture = Texture::fromText(renderer, font, text.c_str(), 0xff, 0xff, 0xff);
    }

    SDL_SetRenderDrawColor(renderer, 0xaa, 0xaa, 0xaa, 0xff);
    SDL_Rect rect = { x, y, w, h };
    SDL_RenderDrawRect(renderer, &rect);

    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    int start, width = 14;
    TTF_SizeText(font, text.substr(0, pos).c_str(), &start, NULL);
    if (text.size() > pos) {
        TTF_SizeText(font, text.substr(0, pos+1).c_str(), &width, NULL);
        width -= start;
    }

    SDL_RenderDrawLine(renderer, x + 9 + start, y + h - 4, x + 9 + start + width, y + h - 4);

    if (texture != NULL)
        texture->render(renderer, x + 10, y);
}
