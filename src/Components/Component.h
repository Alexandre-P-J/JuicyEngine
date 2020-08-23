#pragma once
#include <entt/entt.hpp>
#include <functional>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <spdlog/spdlog.h>
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
    using loaders_map_type =
        std::unordered_map<std::string,
                           std::function<void(nlohmann::json const &,
                                              entt::registry &, entt::entity)>>;
    static loaders_map_type &get_loaders() {
        static loaders_map_type loaders;
        return loaders;
    }

    using serializers_map_type = std::unordered_map<
        entt::id_type,
        std::function<void(nlohmann::json &, entt::registry const &,
                           entt::entity const)>>;
    static serializers_map_type &get_serializers() {
        static serializers_map_type serializers;
        return serializers;
    }

    ComponentFactory() {}

public:
    template <typename Derived>
    static bool registerType() {
        auto &loaders = get_loaders();
        loaders.try_emplace(
            Derived::name,
            [](nlohmann::json const &json, entt::registry &r, entt::entity e) {
                auto& component = r.emplace<Derived>(e);
                json.get_to(component);
            });

        auto &serializers = get_serializers();
        serializers.try_emplace(
            entt::type_info<Derived>::id(),
            [](nlohmann::json &json, entt::registry const &r,
               entt::entity const e) {
                auto const &inst = r.get<Derived>(e);
                json[inst.name] = inst;
            });

        return true;
    }

    static void save(nlohmann::json &json, entt::id_type const id,
                     entt::registry const &registry,
                     entt::entity const entity) {
        auto &serializers = get_serializers();
        auto it = serializers.find(id);
        if (it != serializers.end()) it->second(json, registry, entity);
    }

    static void load(nlohmann::json const &json, std::string const &name,
                     entt::registry &registry, entt::entity entity) {
        auto &loaders = get_loaders();
        auto it = loaders.find(name);
        if (it != loaders.end()) it->second(json, registry, entity);
    }
};

template <typename Derived>
bool ComponentRegistry<Derived>::dummy =
    ComponentFactory::registerType<Derived>();

