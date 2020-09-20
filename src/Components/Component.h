#pragma once
#include <Logging.h>
#include <Serialization.h>
#include <Utils/ECS.h>
#include <functional>
#include <nlohmann/json.hpp>
#include <sol/sol.hpp>
#include <string>
#include <unordered_map>

namespace JuicyEngine {

class Component {
public:
    virtual ~Component() = default;
    virtual void save(nlohmann::json &json) const = 0;
    virtual void load(nlohmann::json const &json) = 0;
};

template <typename Derived>
class ComponentRegistry : public Component {
    static bool dummy;

    template <typename T, T>
    struct dummy_value {};
    typedef dummy_value<bool &, dummy>
        dummy_value_type;  // force dummy to get evaluated
};

class ComponentFactory {
    using loader =
        std::function<void(nlohmann::json const &, Registry &, Entity const)>;
    using serializer =
        std::function<void(nlohmann::json &, Registry const &, Entity const)>;

    using factories_t =
        std::unordered_map<std::string, std::pair<loader, serializer>>;

    static factories_t &get_cpp_factories() {
        static factories_t factories;
        return factories;
    }

    static factories_t &get_lua_factories() {
        static factories_t factories;
        return factories;
    }

    ComponentFactory() = default;

public:
    template <typename Derived>
    static bool register_cpp_component() {
        loader load = [](nlohmann::json const &json, Registry &r,
                         Entity const e) {
            auto &component = r.set_component<Derived>(e, Derived::name);
            json.get_to(component);
        };
        serializer save = [](nlohmann::json &json, Registry const &r,
                             Entity const e) {
            auto inst = r.get<Derived>(e, Derived::name);
            if (inst) json[inst->name] = *inst;
        };

        auto &factories = get_cpp_factories();
        factories.try_emplace(Derived::name, std::make_pair(load, save));
        return true;
    }

    template <class Derived>
    static bool register_lua_component_override_cpp(
        sol::table const data, std::string const &name,
        sol::protected_function data_inherit) {
        sol::table default_data = data_inherit(data);

        loader load = [name, default_data, data_inherit](
                          nlohmann::json const &json, Registry &r,
                          Entity const e) {
            sol::table inherited_data = data_inherit(default_data);
            auto &component =
                r.set_component<Derived>(e, name, inherited_data, name);
            json.get_to(component);
        };
        serializer save = [name](nlohmann::json &json, Registry const &r,
                                 Entity const e) {
            auto inst = r.get<Derived>(e, name);
            if (inst) json[name] = *inst;
        };

        auto &factories = get_lua_factories();
        const auto [it, b] =
            factories.try_emplace(name, std::make_pair(load, save));
        if (!b) {
            Logging::Engine(
                Level::warning,
                "Lua component by the name of " + name +
                    " didn't get registered due to the existence of a "
                    "previous lua component registry with the same name");
        }
        return b;
    }

    template <class Derived>
    static bool register_lua_component(sol::table const data,
                                       std::string const &name,
                                       sol::protected_function data_inherit) {
        auto &cpp_factories = get_cpp_factories();
        if (cpp_factories.find(name) != cpp_factories.end()) {
            Logging::Engine(
                Level::warning,
                "Lua component by the name of " + name +
                    " didn't get registered due an to an existing cpp "
                    "counterpart (you may want to call register_override "
                    "instead)");
            return false;
        }
        return register_lua_component_override_cpp<Derived>(data, name,
                                                            data_inherit);
    }

    static void unregister_lua_components() {
        auto &factories = get_lua_factories();
        factories.clear();
    }

    static void save(nlohmann::json &json, std::string const &name,
                     Registry const &registry, Entity const entity) {
        auto &lua_factories = get_lua_factories();
        auto lua_it = lua_factories.find(name);
        if (lua_it != lua_factories.end()) {
            lua_it->second.second(json, registry, entity);
            return;
        }
        auto &cpp_factories = get_cpp_factories();
        auto cpp_it = cpp_factories.find(name);
        if (cpp_it != cpp_factories.end())
            cpp_it->second.second(json, registry, entity);
    }

    static void load(nlohmann::json const &json, std::string const &name,
                     Registry &registry, Entity const entity) {
        auto &lua_factories = get_lua_factories();
        auto lua_it = lua_factories.find(name);
        if (lua_it != lua_factories.end()) {
            lua_it->second.first(json, registry, entity);
            return;
        }
        auto &cpp_factories = get_cpp_factories();
        auto cpp_it = cpp_factories.find(name);
        if (cpp_it != cpp_factories.end())
            cpp_it->second.first(json, registry, entity);
    }
};

template <typename Derived>
bool ComponentRegistry<Derived>::dummy =
    ComponentFactory::register_cpp_component<Derived>();

}  // namespace JuicyEngine
