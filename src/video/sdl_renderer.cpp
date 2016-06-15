#include "sdl_renderer.h"

#include <iostream>

SDLRenderer::SDLRenderer() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not be initialized: " << SDL_GetError() << std::endl;
        exit(1);
    }

    SDL_CreateWindowAndRenderer(SCREEN_WIDTH * 3, SCREEN_HEIGHT * 3, 0, &window, &renderer);

    if(window == NULL) {
        std::cout << "Window could not be created: " << SDL_GetError() << std::endl;
        exit(1);
    }

    SDL_RenderSetScale(renderer, 3.0, 3.0);
    framebuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STATIC, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void SDLRenderer::render_frame(uint32_t img[]) {
    SDL_UpdateTexture(framebuffer, NULL, img, SCREEN_WIDTH * sizeof(uint32_t));
    SDL_RenderCopy(renderer, framebuffer, NULL, NULL);
    SDL_RenderPresent(renderer);
}

