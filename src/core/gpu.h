#ifndef GPU_H
#define GPU_H

#include <cstdint>

#include <SDL2/SDL.h>

class GPU {
public:
	GPU();

	void step(int cpu_cycles);
	void write_byte(uint16_t addr, uint8_t val);
	void write_word(uint16_t addr, uint16_t val);
	void render_scanline();

	uint8_t get_lcd_control();
	void set_lcd_control(uint8_t val);
	uint8_t get_scroll_x();
	void set_scroll_x(uint8_t val);
	uint8_t get_scroll_y();
	void set_scroll_y(uint8_t val);
	uint8_t get_curr_scanline();
	void set_bg_palette(uint8_t val);

private:
	enum STATE {
		STATE_HORIZONTAL_BLANK,
		STATE_VERTICAL_BLANK,
		STATE_SCANLINE_OAM,
		STATE_SCANLINE_VRAM
	};

	enum CYCLES {
		CYCLES_HORIZONTAL_BLANK = 51,
		CYCLES_VERTICAL_BLANK = 114,
		CYCLES_SCANLINE_OAM = 20,
		CYCLES_SCANLINE_VRAM = 43
	};

	enum Vram {
		VRAM_TILE_SET_1 = 0x0000,
		VRAM_TILE_SET_1_2 = 0x0800,
		VRAM_TILE_SET_0 = 0x1000,
		VRAM_TILE_MAP_0 = 0x1800,
		VRAM_TILE_MAP_1 = 0x1C00
	};

	enum LCD_CONTROL {
		LCD_CONTROL_BG = 0x1,
		LCD_CONTROL_SPRITE = 0x2,
		LCD_CONTROL_SPRITE_SIZE = 0x4,
		LCD_CONTROL_BG_TILE_MAP = 0x8,
		LCD_CONTROL_BG_TILE_SET = 0x10,
		LCD_CONTROL_WINDOW = 0x20,
		LCD_CONTROL_WINDOW_TILE_MAP = 0x40,
		LCD_CONTROL_DISPLAY = 0x80
	};

	const int LAST_LINE = 143;
	const int TILE_WIDTH_PIXELS = 8;
	const int TILE_HEIGHT_PIXELS = 8;
	const int TILES_PER_MAP_ROW = 32;
	const int TILE_SIZE = 16;

	const int SCREEN_WIDTH = 160;

	uint8_t vram[0x2000] = {0};

	int curr_state;
	int state_cycles;

	uint8_t lcd_control;
	uint8_t scroll_y;
	uint8_t scroll_x;
	uint8_t curr_line;

	SDL_Window* window = NULL;
	SDL_Renderer* renderer;
};

#endif