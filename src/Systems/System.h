#pragma once
#include <map>
#include <string>

namespace JuicyEngine {
class System;

class SystemFactory {
    friend class System;
    virtual System* create() = 0;
};

class System {
    static std::map<std::string, SystemFactory*> get_factories() {
        static std::map<std::string, SystemFactory*> factories;
        return factories;
    }

public:
    static void registerType(const std::string& name, SystemFactory* factory) {
        auto factories = get_factories();
        factories[name] = factory;
    }

    static System* create(const std::string& name) {
        auto factories = get_factories();
        for (auto& factory : factories) {
            factory.second->create();
        }
        return factories[name]->create();
    }
};

#define REGISTER_SYSTEM(klass)                                                \
    class klass##Factory : public JuicyEngine::SystemFactory {                \
        klass##Factory() { JuicyEngine::System::registerType(#klass, this); } \
                                                                              \
    public:                                                                   \
        static klass##Factory* get_instance() {                               \
            static klass##Factory* instance = new klass##Factory();           \
            return instance;                                                  \
        }                                                                     \
        virtual JuicyEngine::System* create() { return new klass(); }         \
    };                                                                        \
    static klass##Factory* global_##klass##Factory =                          \
        klass##Factory::get_instance();
}  // namespace JuicyEngine
