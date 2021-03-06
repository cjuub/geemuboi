#pragma once

#include "core/input.h"

#include <SDL2/SDL.h>

namespace geemuboi::input {


class SDLKeyboard {
public:
    SDLKeyboard(geemuboi::core::Input& input_in);
    void update_button_presses();
private:
    enum Keys {
        KEY_X = SDL_SCANCODE_X,
        KEY_Z = SDL_SCANCODE_Z,
        KEY_BS = SDL_SCANCODE_BACKSPACE,
        KEY_RET = SDL_SCANCODE_RETURN,
        KEY_RIGHT = SDL_SCANCODE_RIGHT,
        KEY_LEFT = SDL_SCANCODE_LEFT,
        KEY_UP = SDL_SCANCODE_UP,
        KEY_DOWN = SDL_SCANCODE_DOWN
    };

    geemuboi::core::Input& input;
};


}
