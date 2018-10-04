#include "input.h"
#include "../utils/logger.h"

Input::Input() : buttons_pressed{0x0F, 0x0F}, 
    column_down{}
{}

uint8_t Input::get_buttons_pressed() const {
    if (column_down[0]) {
        return buttons_pressed[0];
    } else if (column_down[1]) {
        return buttons_pressed[1];
    }

    return 0xF;
}

void Input::set_buttons_pressed_switch(uint8_t buttons_pressed) {
    column_down[0] = (buttons_pressed & 0x10);
    column_down[1] = (buttons_pressed & 0x20);
}

void Input::set_buttons_pressed(int column, uint8_t buttons_pressed) {
    this->buttons_pressed[column] = buttons_pressed;
}

bool Input::get_column_down(int column) const {
    return column_down[column];
}

