#pragma once
#include <entt/entt.hpp>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Component {
public:
    virtual ~Component() = default;
    virtual std::string get_name() const = 0;
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
    static std::unordered_map<std::string, std::unique_ptr<Component> (*)()>
        &get_factories() {
        static std::unordered_map<std::string, std::unique_ptr<Component> (*)()>
            derivedFactories;
        return derivedFactories;
    }

    static std::unordered_map<entt::id_type,
                              std::function<std::unique_ptr<Component>(
                                  const entt::registry &, const entt::entity)>>
        &get_id_mappings() {
        static std::unordered_map<
            entt::id_type, std::function<std::unique_ptr<Component>(
                               const entt::registry &, const entt::entity)>>
            mapping;
        return mapping;
    }

    ComponentFactory() {}

public:
    template <typename Derived>
    static bool registerType() {
        auto &factories = get_factories();
        factories.try_emplace(Derived::name, []() {
            return std::unique_ptr<Component>(new Derived);
        });

        auto &mapping = get_id_mappings();
        mapping.try_emplace(entt::type_info<Derived>::id(),
                            [](const entt::registry &r, const entt::entity e) {
                                return std::unique_ptr<Component>(
                                    new Derived(r.get<Derived>(e)));
                            });
        return true;
    }

    static std::unique_ptr<Component> create(const std::string &name) {
        auto &factories = get_factories();
        auto it = factories.find(name);
        if (it != factories.end())
            return it->second();
        else
            return std::unique_ptr<Component>();  // invalid request
    }

    static auto get_from_id(const entt::id_type id,
                            const entt::registry &registry,
                            const entt::entity entity) {
        auto &mapping = get_id_mappings();
        auto it = mapping.find(id);
        if (it != mapping.end())
            return it->second(registry, entity);
        else
            return std::unique_ptr<Component>();  // invalid request
    }
};

template <typename Derived>
bool ComponentRegistry<Derived>::dummy =
    ComponentFactory::registerType<Derived>();

