#include <Game.h>

void Game::update() {
    pre_update();
    if (root) {
        root->propagate_update();
    }
    post_update();
}



void Game::set_root_node(std::shared_ptr<Node> node) {
    root = node; // ths may delete the old nodetree automatically
}