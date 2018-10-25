#include "view/renderer.h"

namespace geemuboi::test::view {


class MockRenderer : public geemuboi::view::Renderer {
public:
    MOCK_METHOD1(render_frame, void(uint32_t img[]));
};


}
