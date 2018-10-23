#include "core/gpu.h"


GPU::GPU(Renderer& renderer_in) : vram{},
    oam{},
    curr_state{},
    state_cycles{},
    lcd_control{},
    scroll_y{},
    scroll_x{},
    curr_line{},
    bg_palette{},
    renderer(renderer_in),
    framebuffer{}
{}

void GPU::step(int cpu_cycles) {
    state_cycles += cpu_cycles;

    switch (curr_state) {
    case STATE_HORIZONTAL_BLANK:
        if (state_cycles >= CYCLES_HORIZONTAL_BLANK) {
            curr_state = (curr_line == LAST_LINE) ? STATE_VERTICAL_BLANK : STATE_SCANLINE_OAM;
            ++curr_line;
            state_cycles = 0;
        }

        break;
    case STATE_VERTICAL_BLANK:
        if (state_cycles >= CYCLES_VERTICAL_BLANK) {
            ++curr_line;
            state_cycles = 0;

            if (curr_line > VBLANK_LAST_LINE) {
                curr_line = 0;
                curr_state = STATE_SCANLINE_OAM;

                renderer.render_frame(framebuffer);
            }
        }

        break;
    case STATE_SCANLINE_OAM:
        if (state_cycles >= CYCLES_SCANLINE_OAM) {
            curr_state = STATE_SCANLINE_VRAM;
            state_cycles = 0;
        }

        break;
    case STATE_SCANLINE_VRAM:
        if (state_cycles >= CYCLES_SCANLINE_VRAM) {
            curr_state = STATE_HORIZONTAL_BLANK;
            state_cycles = 0;

            render_scanline();
        }
    }
}

void GPU::render_scanline() {
    if (lcd_control & LCD_CONTROL_BG_ENABLE) {
        render_background();
    }

    if (lcd_control & LCD_CONTROL_SPRITE_ENABLE) {
        render_sprites();
    }
}

void GPU::render_background() {
    uint16_t map_addr;
    map_addr = (lcd_control & LCD_CONTROL_BG_TILE_MAP) ? VRAM_TILE_MAP_1 : VRAM_TILE_MAP_0;

    uint16_t map_offset_y = static_cast<uint8_t>(curr_line + scroll_y) / TILE_HEIGHT_PIXELS;
    map_addr += map_offset_y * TILES_PER_MAP_ROW;

    int map_offset_x = scroll_x / TILE_WIDTH_PIXELS;
    map_addr += map_offset_x;

    uint16_t tile_set_addr;
    tile_set_addr = (lcd_control & LCD_CONTROL_BG_TILE_SET) ? VRAM_TILE_SET_1 : VRAM_TILE_SET_0;

    if (lcd_control & LCD_CONTROL_BG_TILE_SET) {
        // tile_nbr += 256;
    }

    int tile_nbr;
    int initial_tile_x = scroll_x & 0x7;
    int tile_y = (curr_line + scroll_y) & 0x7;
    
    for (int i = 0; i < Renderer::SCREEN_WIDTH; ++i) {
        int tile_x = (initial_tile_x + i) % TILE_WIDTH_PIXELS;
        if (tile_x == 0) {
            tile_nbr = static_cast<int8_t>(vram[map_addr++]);
        }

        uint8_t low = vram[tile_set_addr + TILE_SIZE * tile_nbr + tile_y * 2];
        uint8_t high = vram[tile_set_addr + TILE_SIZE * tile_nbr + tile_y * 2 + 1];
        
        low = (low >> (7 - tile_x)) & 0x1;
        high = (high >> (6 - tile_x)) & 0x2;

        uint8_t color = high + low;
        color = (bg_palette >> (color * 2)) & 0x3;

        uint32_t pixel = 0;
        switch (color) {
            case 0: pixel = PIXEL_COLOR_WHITE; break;
            case 1: pixel = PIXEL_COLOR_LIGHT_GREY; break;
            case 2: pixel = PIXEL_COLOR_DARK_GREY; break;
            case 3: pixel = PIXEL_COLOR_BLACK; break;
        }

        framebuffer[i + curr_line * Renderer::SCREEN_WIDTH] = pixel;
    }
}

void GPU::render_sprites() {
    // for each sprite
    for (int i = 0; i != NBR_OAMS * OAM_SIZE; i += OAM_SIZE) {
        OamEntry sprite = {oam[i], oam[i + 1], oam[i + 2], oam[i + 3]};
        // if this sprite is on current line
        if (sprite.y - 16 <= curr_line && sprite.y - 16 + TILE_HEIGHT_PIXELS > curr_line) {
            uint8_t palette = (sprite.flags & 0x10) ? obj_palette[1] : obj_palette[0];
            int priority = sprite.flags & 0x80;

            int tile_y = (curr_line + scroll_y) & 0x7;
            uint16_t tile_set_addr;
            tile_set_addr = (lcd_control & LCD_CONTROL_BG_TILE_SET) ? VRAM_TILE_SET_1 : VRAM_TILE_SET_0;

            // TODO PRINT TILE SEE IF CORRECT


            for (int x = 0; x != TILE_WIDTH_PIXELS; ++x) {
                //uint8_t low = vram[tile_set_addr + TILE_SIZE * sprite.tile_nbr + tile_y * 2 + x * 2];
                //uint8_t high = vram[tile_set_addr + TILE_SIZE * sprite.tile_nbr + tile_y * 2 +x*2+1];

                // (nbr * (tile_rows * byte_per_row)) + (

                // VIEW VRAM, CHECK WITH OTHER EMU

                uint8_t low = vram[3 * 8 * 2 + 2 + x * 2];
                uint8_t high = vram[3 * 8 * 2 + 2 + x * 2 + 1];

                low = (low >> (7 - x)) & 0x1;
                high = (high >> (6 - x)) & 0x2;

                uint8_t color = high + low;
                color = (palette >> (color * 2)) & 0x3;


                uint32_t pixel = 0;
                switch (color) {
                    case 0: pixel = PIXEL_COLOR_WHITE; break;
                    case 1: pixel = PIXEL_COLOR_LIGHT_GREY; break;
                    case 2: pixel = PIXEL_COLOR_DARK_GREY; break;
                    case 3: pixel = PIXEL_COLOR_BLACK; break;
                }

                framebuffer[sprite.x - 8 + x + curr_line * Renderer::SCREEN_WIDTH] = pixel;
            }
        }
    }
}

void GPU::write_byte_vram(uint16_t addr, uint8_t val) {
    vram[addr] = val;
}

void GPU::write_word_vram(uint16_t addr, uint16_t val) {
    vram[addr] = val;
    vram[addr + 1] = val >> 8;
}

uint8_t GPU::read_byte_oam(uint16_t addr) const {
    return oam[addr];
}

void GPU::write_byte_oam(uint16_t addr, uint8_t val) {
    oam[addr] = val;
}

uint16_t GPU::read_word_oam(uint16_t addr) const {
    return oam[addr] + (oam[addr + 1] << 8);
}

void GPU::write_word_oam(uint16_t addr, uint16_t val) {
    oam[addr] = val;
    oam[addr + 1] = val >> 8;
}

uint8_t GPU::get_lcd_control() {
    return lcd_control;
}

void GPU::set_lcd_control(uint8_t val) {
    lcd_control = val;
}

uint8_t GPU::get_scroll_x() {
    return scroll_x;
}

void GPU::set_scroll_x(uint8_t val) {
    scroll_x = val;
}

uint8_t GPU::get_scroll_y() {
    return scroll_y;
}

void GPU::set_scroll_y(uint8_t val) {
    scroll_y = val;
}




uint8_t GPU::get_curr_scanline() {
    return curr_line;
}

void GPU::set_bg_palette(uint8_t val) {
    bg_palette = val;
}

uint8_t GPU::get_obj_palette(int index) {
    return obj_palette[index];
}

void GPU::set_obj_palette(int index, uint8_t val) {
    obj_palette[index] = val;
}

