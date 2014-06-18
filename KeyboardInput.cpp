#include "KeyboardInput.h"
#include "utility.h"

KeyboardInput::KeyboardInput(SDL_Scancode up, SDL_Scancode down) : upKey(up), downKey(down) {}

int KeyboardInput::update(Entity& paddle) {
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    return state[downKey] - state[upKey];
}
