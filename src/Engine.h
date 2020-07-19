#pragma once
#include <Systems/RenderSystem.h>
#include <memory>
#include "Game.h"

namespace JuicyEngine {
class Engine {
    RenderSystem renderer;
    std::shared_ptr<Game> game;
    bool running = false;

    Engine();

public:
    ~Engine();
    static std::shared_ptr<Engine> get_instance();
    void run(Game* game);
};
}  // namespace JuicyEngine
