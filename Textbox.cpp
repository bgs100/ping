#include <algorithm>
#include "Textbox.h"

Textbox::Textbox(TTF_Font *font, int x, int y, int w)
    : font(font), x(x), y(y), w(w), h(TTF_FontHeight(font)), renderText(false), pos(0), offsetX(0) {
    SDL_StartTextInput();
}

Textbox::~Textbox() {
    SDL_StopTextInput();
}

void Textbox::clear() {
    text.erase();
    pos = 0;
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
        texture = Texture::fromText(renderer, font, text.c_str());
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

    if (start + width - w + 20 >= offsetX)
        offsetX = start + width - w + 20;
    else if (start < offsetX)
        offsetX = start;

    int fullSize;
    TTF_SizeText(font, text.c_str(), &fullSize, NULL);
    offsetX = std::max(0, std::min(offsetX, fullSize - w + 20 + (pos == text.size() && texture.w > w - 20 ? width : 0)));
    texture.render(renderer, offsetX, 0, std::min(texture.w - offsetX, w - 20), texture.h, x + 10, y);

    SDL_RenderDrawLine(renderer, x + 9 + start - offsetX, y + h - 4, x + 9 + start + width - offsetX, y + h - 4);
}
