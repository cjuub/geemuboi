#ifndef GPU_H
#define GPU_H

#include "../view/renderer.h"

#include <cstdint>

class GPU {
public:
    GPU(Renderer& renderer_in);

    void step(int cpu_cycles);
    void write_byte_vram(uint16_t addr, uint8_t val);
    void write_word_vram(uint16_t addr, uint16_t val);
    uint8_t read_byte_oam(uint16_t addr) const;
    void write_byte_oam(uint16_t addr, uint8_t val);
    uint16_t read_word_oam(uint16_t addr) const;
    void write_word_oam(uint16_t addr, uint16_t val);
    void render_scanline();

    // TODO read_register(addr) instead of getters and move reg constants to here
    // probably ignore above as logic in mmu becomes more uniform now
    uint8_t get_lcd_control();
    void set_lcd_control(uint8_t val);
    uint8_t get_scroll_x();
    void set_scroll_x(uint8_t val);
    uint8_t get_scroll_y();
    void set_scroll_y(uint8_t val);
    uint8_t get_curr_scanline();
    void set_bg_palette(uint8_t val);
    uint8_t get_obj_palette(int index);
    void set_obj_palette(int index, uint8_t val);

    enum Cycles {
        CYCLES_HORIZONTAL_BLANK = 51,
        CYCLES_VERTICAL_BLANK = 114,
        CYCLES_SCANLINE_OAM = 20,
        CYCLES_SCANLINE_VRAM = 43,
        CYCLES_PER_FRAME = 70224
    };
private:
    enum States {
        STATE_HORIZONTAL_BLANK,
        STATE_VERTICAL_BLANK,
        STATE_SCANLINE_OAM,
        STATE_SCANLINE_VRAM
    };

    enum VRAM {
        VRAM_TILE_SET_1 = 0x0000,
        VRAM_TILE_SET_1_2 = 0x0800,
        VRAM_TILE_SET_0 = 0x1000,
        VRAM_TILE_MAP_0 = 0x1800,
        VRAM_TILE_MAP_1 = 0x1C00
    };

    enum LCDControlFlags {
        LCD_CONTROL_BG_ENABLE = 0x1,
        LCD_CONTROL_SPRITE_ENABLE = 0x2,
        LCD_CONTROL_SPRITE_SIZE = 0x4,
        LCD_CONTROL_BG_TILE_MAP = 0x8,
        LCD_CONTROL_BG_TILE_SET = 0x10,
        LCD_CONTROL_WINDOW = 0x20,
        LCD_CONTROL_WINDOW_TILE_MAP = 0x40,
        LCD_CONTROL_DISPLAY = 0x80
    };

    enum PixelColors {
        PIXEL_COLOR_WHITE = 0x00FFFFFF,
        PIXEL_COLOR_LIGHT_GREY = 0x00C0C0C0,
        PIXEL_COLOR_DARK_GREY = 0x005C5C5C,
        PIXEL_COLOR_BLACK = 0x00000000
    };

    static const int LAST_LINE = 143;
    static const int VBLANK_LAST_LINE = 153;

    static const int TILE_WIDTH_PIXELS = 8;
    static const int TILE_HEIGHT_PIXELS = 8;
    static const int TILES_PER_MAP_ROW = 32;
    static const int TILE_SIZE = 16;

    static constexpr int NBR_OAMS = 40;
    static constexpr int OAM_SIZE = 4;

    struct OamEntry {
        uint8_t y;
        uint8_t x;
        uint8_t tile_nbr;
        uint8_t flags;
    };

    void render_background();
    void render_sprites();

    uint8_t vram[0x2000];
    uint8_t oam[NBR_OAMS * OAM_SIZE];

    int curr_state;
    int state_cycles;

    uint8_t lcd_control;
    uint8_t scroll_y;
    uint8_t scroll_x;
    uint8_t curr_line;
    uint8_t bg_palette;
    uint8_t obj_palette[2];

    Renderer& renderer;
    uint32_t framebuffer[Renderer::SCREEN_WIDTH * Renderer::SCREEN_HEIGHT];
};

#endif
