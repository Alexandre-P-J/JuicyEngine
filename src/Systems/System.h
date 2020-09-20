#pragma once
#include <Logging.h>
#include <Utils/ECS.h>
#include <functional>
#include <iostream>
#include <memory>
#include <sol/sol.hpp>
#include <string>
#include <unordered_map>
#include <vector>

namespace JuicyEngine {

class System {
public:
    virtual ~System() = default;
    virtual void update(Registry &scene) = 0;
};

template <typename Derived>
class SystemRegistry : public System {
public:
    static bool dummy;

    template <typename T, T>
    struct dummy_value {};
    typedef dummy_value<bool &, dummy>
        dummy_value_type;  // force dummy to get evaluated
};

class SystemFactory {
    using factory_t = std::function<std::unique_ptr<System>()>;
    static std::unordered_map<std::string, factory_t> &get_cpp_factories() {
        static std::unordered_map<std::string, factory_t> factories;
        return factories;
    }
    static std::unordered_map<std::string, factory_t> &get_lua_factories() {
        static std::unordered_map<std::string, factory_t> factories;
        return factories;
    }

    SystemFactory() = default;

public:
    template <typename Derived>
    static bool register_cpp_system() {
        auto &factories = get_cpp_factories();
        factories.try_emplace(Derived::name, []() {
            return std::unique_ptr<System>(new Derived);
        });
        return true;
    }

    template <class Derived>
    static void register_lua_system(sol::protected_function const func,
                                    std::string const &name) {
        auto &cpp_factories = get_cpp_factories();
        if (cpp_factories.find(name) != cpp_factories.end()) {
            Logging::Engine(
                Level::warning,
                "Lua system by the name of " + name +
                    " didn't get registered due an to an existing cpp "
                    "counterpart (you may want to call register_override "
                    "instead)");
            return;
        }
        register_lua_system_override_cpp<Derived>(func, name);
    }

    template <class Derived>
    static void register_lua_system_override_cpp(
        sol::protected_function const func, std::string const &name) {
        auto &factories = get_lua_factories();
        const auto [it, b] = factories.try_emplace(name, [func, name]() {
            return std::unique_ptr<System>(new Derived(func, name));
        });
        if (!b) {
            Logging::Engine(
                Level::warning,
                "Lua system by the name of " + name +
                    " didn't get registered due to the existence of a "
                    "previous lua system registry with the same name");
        }
    }

    static std::unique_ptr<System> create(const std::string &name) {
        auto &lua_factories = get_lua_factories();
        auto lua_it = lua_factories.find(name);
        if (lua_it != lua_factories.end()) return lua_it->second();
        auto &cpp_factories = get_cpp_factories();
        auto cpp_it = cpp_factories.find(name);
        if (cpp_it != cpp_factories.end()) return cpp_it->second();
        return std::unique_ptr<System>();  // invalid request
    }

    static void unregister_lua_systems() {
        auto &factories = get_lua_factories();
        factories.clear();
    }
};

template <typename Derived>
bool SystemRegistry<Derived>::dummy =
    SystemFactory::register_cpp_system<Derived>();

}  // namespace JuicyEngine
