#include "ButtonMenu.h"
#include "Texture.h"

// horizCenter is false by default (see ButtonMenu.h)
ButtonMenu::ButtonMenu(SDL_Renderer *renderer, TTF_Font *font, const char *labels[], int numLabels, int x, int y, int spacing, bool horizCenter)
    : selectedIndex(-1), fontHeight(TTF_FontHeight(font)), numButtons(numLabels), menuX(x), menuY(y), spacing(spacing), centered(horizCenter) {
    buttons = new Texture[numLabels];
    for (int i = 0; i < numButtons; i++)
        buttons[i] = Texture::fromText(renderer, font, labels[i]);
}

ButtonMenu::~ButtonMenu() {
    delete[] buttons;
}

void ButtonMenu::render(SDL_Renderer *renderer) {
    int size = fontHeight + spacing;
    for (int i = 0; i < numButtons; i++) {
        int x = menuX;

        if (centered)
            x -= buttons[i].w/2;

        if (i != selectedIndex)
            buttons[i].setAlphaMod(0xbb);

        buttons[i].render(renderer, x, menuY + i*size);
        buttons[i].setAlphaMod(0xff);
    }
}

int ButtonMenu::getButton(int x, int y) {
    int size = fontHeight + spacing;
    if ((centered || x >= menuX) && y >= menuY && y < menuY + numButtons*size && (y-menuY) % size <= fontHeight) {
        int button = (y - menuY) / size;
        if ((!centered && x - menuX < buttons[button].w) || (x >= menuX - buttons[button].w/2 && x < menuX + buttons[button].w/2))
            return button;
    }

    return -1;
}

void ButtonMenu::selectButton(int x, int y) {
    selectedIndex = getButton(x, y);
}

int ButtonMenu::getSelected() {
    return selectedIndex;
}
