#include <Engine.h>
#include <Sprite.h>
#include <spdlog/spdlog.h>
#include <iostream>

#include <sol/sol.hpp>

class Mygame : public Game {
    std::shared_ptr<Sprite> a = std::shared_ptr<Sprite>(new Sprite());
    Sprite b;
    std::shared_ptr<Camera> c = std::shared_ptr<Camera>(new Camera(true));

    void start() {
        set_root_node(a);
        b.set_parent(a);
        b.translate(glm::vec2(50,50));
        a->translate(glm::vec2(50,50));
    }

    void post_update() {
        //Renderer::get_instance()->set_current_view();
        a->draw();
        b.draw();
        c->translate(glm::vec2(0.6,0));
    }
};


int main(int argc, char* args[]) {
    auto engine = Engine::get_instance();
    engine->run(new Mygame());
    return 0;
}
