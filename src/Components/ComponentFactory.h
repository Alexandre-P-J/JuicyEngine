#pragma once
#include <Components/Component.h>
#include <Components/ScriptedComponent.h>

class ComponentFactory {
    // friend ComponentRegistry so it has access to registerType
    template <class Derived>
    friend class ComponentRegistry;

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

    ComponentFactory() = default;

    template <typename Derived>
    static bool registerType() {
        std::cout << "REGISTER Component" << std::endl;
        auto &loaders = get_loaders();
        loaders.try_emplace(
            Derived::name,
            [](nlohmann::json const &json, entt::registry &r, entt::entity e) {
                auto &component = r.emplace<Derived>(e);
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

public:
    static void save(nlohmann::json &json, entt::id_type const id,
                     entt::registry const &registry,
                     entt::entity const entity) {
        auto &serializers = get_serializers();
        auto it = serializers.find(id);
        if (it != serializers.end()) it->second(json, registry, entity);
    }

    static void load(nlohmann::json const &json, std::string const &name,
                     entt::registry &registry, entt::entity entity) {
        if (name.size() > 4) {
            if (name.substr(name.size() - 4, 4) == ".lua") {
                spdlog::warn(name);
                return;
                // might be a lua system
            }
        }

        auto &loaders = get_loaders();
        auto it = loaders.find(name);
        if (it != loaders.end()) it->second(json, registry, entity);
    }

    //inline static bool has_all(std::vector<std::string component>){
        //auto& m = get_loaders();
        //return
    //}
};

template <typename Derived>
bool ComponentRegistry<Derived>::dummy =
    ComponentFactory::registerType<Derived>();

