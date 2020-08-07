#pragma once
#include <Nodes/Node.h>
#include <memory>

namespace JuicyEngine {
class SceneManager {
    friend class Engine;
    std::shared_ptr<Node> root;
};
}  // namespace JuicyEngine
