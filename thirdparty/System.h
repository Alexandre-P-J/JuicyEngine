#pragma once
#include <spdlog/spdlog.h>
#include <map>
#include <string>
#include <vector>

namespace JuicyEngine {
class System;

class SystemFactory {
    friend class System;

public:
    virtual System* create() = 0;
};

class System {
    static std::vector<SystemFactory*>& get_factories() {
        static std::vector<SystemFactory*> factories;
        return factories;
    }

public:
    //virtual void update(entt::registry& scene) = 0;

    static void registerType(SystemFactory* factory) {
        auto& factories = get_factories();
        factories.push_back(factory);
    }

    static auto test() {
        return get_factories();
    }

};

}  // namespace JuicyEngine
