#pragma once
#include <Systems/System.h>
#include <entt/entt.hpp>

namespace JuicyEngine {
class RenderSystem : public SystemRegistry<RenderSystem> {
public:
    static constexpr const char* name = "Render";
    void update(entt::registry& scene) override;

};
}  // namespace JuicyEngine

