#include <Engine.h>
#include <spdlog/spdlog.h>



Engine::Engine() {
    SDL_Init(0);
    renderer = Renderer::get_instance();
}




Engine::~Engine() {
    SDL_Quit();
}



std::shared_ptr<Engine> Engine::get_instance() {
    static std::shared_ptr<Engine> ptr(new Engine);
    return ptr;
}




void Engine::run(Game* game_ptr) {
    if (running) {
        spdlog::warn("Do not call Engine::run() multiple times!");
        return;
    }
    running = true;
    game = std::shared_ptr<Game>(game_ptr);
    game->start();
    while(running) {
        game->update();
        renderer->update();
    }
    game->finish();
}