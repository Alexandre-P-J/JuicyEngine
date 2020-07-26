#pragma once
#include <Systems/System.h>
#include <entt/entt.hpp>

namespace JuicyEngine {
class RenderSystem : public SystemRegistry<RenderSystem> {
    friend class Engine;

protected:
    void update(entt::registry& scene) override;

public:
    static constexpr const char* name = "Render";
    uint16_t width = 500, height = 500;
    RenderSystem();
    ~RenderSystem();
};
}  // namespace JuicyEngine

