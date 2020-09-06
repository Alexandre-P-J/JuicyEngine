#pragma once
#include <Systems/System.h>

namespace JuicyEngine {
class RenderSystem : public SystemRegistry<RenderSystem> {
public:
    static constexpr const char* name = "Render";
    void update(Registry& scene) override;

};
}  // namespace JuicyEngine

