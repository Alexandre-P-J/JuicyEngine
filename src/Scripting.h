#pragma once
#include "Node.h"
#include <sol/sol.hpp>

class Scripting {
    sol::state state;

    Scripting();
    void script_panic(sol::optional<std::string> maybe_msg);
    void register_all();

public:
    static std::shared_ptr<Scripting> get_instance();
    inline void context_switch(Node* owner) {
        state["self"] = owner;
    }
    void run_script(Node* owner, const std::string& script);
};