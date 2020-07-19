#include "Game.h"


entt::registry& Game::get_scene() {
    return current_scene;
}


void Game::set_scene(entt::registry& scene) {
    current_scene.clear();
    current_scene = std::move(scene);
}


void Game::update() {

}
