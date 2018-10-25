#ifndef RENDERER_H
#define RENDERER_H

#include <cstdint>

namespace geemuboi::view {


class Renderer {
public:
    static const int SCREEN_WIDTH = 160;
    static const int SCREEN_HEIGHT = 144;

    virtual ~Renderer() {}
    virtual void render_frame(uint32_t img[]) = 0;
};


}

#endif
