#pragma once
#include <Utils/ECS.h>
#include <spdlog/spdlog.h>
#include <functional>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>



namespace JuicyEngine {

template <class T>
void to_json(nlohmann::json &json, const T &obj) {
    obj.save(json);
}

template <class T>
void from_json(nlohmann::json const &json, T &obj) {
    obj.load(json);
}


class Component {
public:
    virtual ~Component() = default;
    virtual std::string get_name() const = 0;
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

public:
    std::string get_name() const override { return Derived::name; }
};

class ComponentFactory {
    // friend ComponentRegistry so it has access to registerType
    template <class Derived>
    friend class ComponentRegistry;

    using loader =
        std::function<void(nlohmann::json const &, Registry &, Entity const)>;
    using serializer =
        std::function<void(nlohmann::json &, Registry const &, Entity const)>;

    using factories_t =
        std::unordered_map<std::string, std::pair<loader, serializer>>;

    static factories_t &get_factories() {
        static factories_t factories;
        return factories;
    }

    ComponentFactory() = default;

    template <typename Derived>
    static bool registerType() {
        loader load = [](nlohmann::json const &json, Registry &r,
                         Entity const e) {
            auto &component = r.set_component<Derived>(e, Derived::name);
            json.get_to(component);
        };
        serializer save = [](nlohmann::json &json, Registry const &r,
                             Entity const e) {
            auto inst = r.get<Derived>(e, Derived::name);
            if (inst)
                json[inst->name] = *inst;
        };

        auto &factories = get_factories();
        factories.try_emplace(Derived::name, std::make_pair(load, save));
        return true;
    }

public:
    static void save(nlohmann::json &json, std::string const &name,
                     Registry const &registry, Entity const entity) {
        auto &factories = get_factories();
        auto it = factories.find(name);
        if (it != factories.end()) it->second.second(json, registry, entity);
    }

    static void load(nlohmann::json const &json, std::string const &name,
                     Registry &registry, Entity const entity) {
        auto &factories = get_factories();
        auto it = factories.find(name);
        if (it != factories.end()) it->second.first(json, registry, entity);
    }
};

template <typename Derived>
bool ComponentRegistry<Derived>::dummy =
    ComponentFactory::registerType<Derived>();

}  // namespace JuicyEngine
