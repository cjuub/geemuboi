#include "gpu.h"

GPU::GPU () {

    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        exit(1);
    }

    SDL_CreateWindowAndRenderer(SCREEN_WIDTH * 3, SCREEN_HEIGHT * 3, 0, &window, &renderer);

    if( window == NULL )
    {
        printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
        exit(1);
    }

    SDL_RenderSetScale(renderer, 3.0, 3.0);
}

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
                SDL_RenderPresent(renderer);
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
    
    for (int i = 0; i < SCREEN_WIDTH; ++i) {
        int tile_x = (initial_tile_x + i) % TILE_WIDTH_PIXELS;
        if (tile_x == 0) {
            tile_nbr = static_cast<int8_t>(vram[map_addr++]);
        }

        uint8_t low = vram[tile_set_addr + TILE_SIZE * tile_nbr + tile_y * 2];
        uint8_t high = vram[tile_set_addr + TILE_SIZE * tile_nbr + tile_y * 2 + 1];
        
        low = (low >> (7 - tile_x)) & 0x1;
        high = (high >> (6 - tile_x)) & 0x2;

        uint8_t color = high + low;

        if (color != 0) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
        }

        SDL_RenderDrawPoint(renderer, i, curr_line);
    }
}

void GPU::write_byte(uint16_t addr, uint8_t val) {
    vram[addr] = val;
}

void GPU::write_word(uint16_t addr, uint16_t val) {
    vram[addr] = val;
    vram[addr + 1] = val >> 8;
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

}

