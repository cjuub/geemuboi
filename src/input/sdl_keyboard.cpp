#include "sdl_keyboard.h"

#include <SDL2/SDL.h>

SDLKeyboard::SDLKeyboard(Input& input_in) : input(input_in) {}

void SDLKeyboard::update_button_presses() {
    const Uint8* state = SDL_GetKeyboardState(NULL);

    uint8_t col1 = 0xF;
    col1 = state[KEY_X] ? col1 & Input::BUTTON_A : col1 | (~Input::BUTTON_A & 0xF);
    col1 = state[KEY_Z] ? col1 & Input::BUTTON_B : col1 | (~Input::BUTTON_B & 0xF);
    col1 = state[KEY_BS] ? col1 & Input::BUTTON_SELECT : col1 | (~Input::BUTTON_SELECT & 0xF);
    col1 = state[KEY_RET] ? col1 & Input::BUTTON_START : col1 | (~Input::BUTTON_START & 0xF);
    input.set_buttons_pressed(0, col1);

    uint8_t col2 = 0xF;
    col2 = state[KEY_RIGHT] ? col2 & Input::BUTTON_RIGHT : col2 | (~Input::BUTTON_RIGHT & 0xF);
    col2 = state[KEY_LEFT] ? (col2 & Input::BUTTON_LEFT) : col2 | (~Input::BUTTON_LEFT & 0xF);
    col2 = state[KEY_UP] ? col2 & Input::BUTTON_UP : col2 | (~Input::BUTTON_UP & 0xF);
    col2 = state[KEY_DOWN] ? col2 & Input::BUTTON_DOWN : col2 | (~Input::BUTTON_DOWN & 0xF);
    input.set_buttons_pressed(1, col2);
}

