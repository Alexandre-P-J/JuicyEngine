#pragma once
#include "Renderer.h"
#include "Game.h"
#include <memory>

class Engine {
    std::shared_ptr<Renderer> renderer;
    std::shared_ptr<Game> game;
    bool running = false;

    Engine();

public:
    ~Engine();
    static std::shared_ptr<Engine> get_instance();
    void run(Game* game);
};