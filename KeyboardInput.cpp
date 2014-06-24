#include "KeyboardInput.h"

KeyboardInput::KeyboardInput(SDL_Scancode up, SDL_Scancode down) : upKey(up), downKey(down) {}

int KeyboardInput::update(SharedState &state, int player) {
    const Uint8 *keyState = SDL_GetKeyboardState(NULL);
    return keyState[downKey] - keyState[upKey];
}
