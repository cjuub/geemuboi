#include "sdl_keyboard.h"

#include <SDL2/SDL.h>

SDLKeyboard::SDLKeyboard(Input& input_in) : input(input_in) {}

void SDLKeyboard::update_button_presses() {
    const Uint8* state = SDL_GetKeyboardState(NULL);

    uint8_t down = input.get_buttons_pressed();
    if (input.get_column_down(0)) {
        down = state[KEY_X] ? down & Input::BUTTON_A : down | (~Input::BUTTON_A & 0xF);
        down = state[KEY_Z] ? down & Input::BUTTON_B : down | (~Input::BUTTON_B & 0xF);
        down = state[KEY_BS] ? down & Input::BUTTON_SELECT : down | (~Input::BUTTON_SELECT & 0xF);
        down = state[KEY_RET] ? down & Input::BUTTON_START : down | (~Input::BUTTON_START & 0xF);
    }

    if (input.get_column_down(1)) {
        down = state[KEY_RIGHT] ? down & Input::BUTTON_RIGHT : down | (~Input::BUTTON_RIGHT & 0xF);
        down = state[KEY_LEFT] ? (down & Input::BUTTON_LEFT) : down | (~Input::BUTTON_LEFT & 0xF);
        down = state[KEY_UP] ? down & Input::BUTTON_UP : down | (~Input::BUTTON_UP & 0xF);
        down = state[KEY_DOWN] ? down & Input::BUTTON_DOWN : down | (~Input::BUTTON_DOWN & 0xF);
    }

    input.set_buttons_pressed(down);
}

