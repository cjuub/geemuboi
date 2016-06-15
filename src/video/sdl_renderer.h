#ifndef SDL_RENDERER_H
#define SDL_RENDERER_H

#include "renderer.h"

#include <SDL2/SDL.h>

class SDLRenderer : public Renderer {
public:
    SDLRenderer();
    void render_frame(uint32_t img[]);
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* framebuffer;
};

#endif

