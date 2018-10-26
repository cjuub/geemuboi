#pragma once

#include "view/renderer.h"

#include <SDL2/SDL.h>

namespace geemuboi::view {


class SDLRenderer : public Renderer {
public:
    SDLRenderer();
    void render_frame(uint32_t img[]);
    void update_fps_indicator(int frames);
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* framebuffer;
};


}
