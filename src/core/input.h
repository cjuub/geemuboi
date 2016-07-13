#ifndef INPUT_H
#define INPUT_H

#include <cstdint>

class Input {
public:
    Input();

    uint8_t get_buttons_pressed() const;
    void set_buttons_pressed(uint8_t buttons_pressed);
    bool get_column_down(int column) const;

    enum Buttons {
        BUTTON_A = 0xE,
        BUTTON_B = 0xD,
        BUTTON_SELECT = 0xB,
        BUTTON_START = 0x7,
        BUTTON_RIGHT = 0xE,
        BUTTON_LEFT = 0xD,
        BUTTON_UP = 0xB,
        BUTTON_DOWN = 0x7
    };

private:
    uint8_t buttons_pressed;
    bool column_down[2];
};

#endif
