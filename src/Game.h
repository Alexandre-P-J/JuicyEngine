#pragma once
#include "Node.h"
#include <memory>
#include "Scripting.h"


class Game {
    friend class Engine;

    std::shared_ptr<Node> root = nullptr;
    std::shared_ptr<Scripting> scripting = Scripting::get_instance();

    void update();
    virtual void start() {};
    virtual void pre_update() {};
    virtual void post_update() {};
    virtual void input() {}; // WIP
    virtual void finish() {};

public:
    void set_root_node(std::shared_ptr<Node> node);
};