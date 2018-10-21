#include "view/renderer.h"

class MockRenderer : public Renderer {
public:
    MOCK_METHOD1(render_frame, void(uint32_t img[]));
};
