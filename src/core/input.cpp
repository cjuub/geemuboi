#include "input.h"

Input::Input() : buttons_pressed{0x0F}, 
    column_down{}
{}

uint8_t Input::get_buttons_pressed() const {
    return buttons_pressed;
}

void Input::set_buttons_pressed(uint8_t buttons_pressed) {
    column_down[0] = (buttons_pressed & 0x10);
    column_down[1] = (buttons_pressed & 0x20);
    if (column_down[0] || column_down[1]) {
        return;
    }

    this->buttons_pressed = buttons_pressed;
}

bool Input::get_column_down(int column) const {
    return column_down[column];
}

