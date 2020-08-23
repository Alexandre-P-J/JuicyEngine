#pragma once
#include <cstdint>

namespace JuicyEngine {

class RenderManager {
    friend class Engine;

    uint16_t width = 500, height = 500;

    RenderManager();

public:
    ~RenderManager();
};

}  // namespace JuicyEngine
