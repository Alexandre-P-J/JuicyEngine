#pragma once
#include <entt/entt.hpp>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "cereal/archives/portable_binary.hpp"
#undef Bool
#include <cereal/archives/json.hpp>
#include <cereal/cereal.hpp>
#include <tuple>

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
    // one dictionary to store factories, another one to store serialization
    // functions, the serialization lambdas could be generalized with a
    // template, but not until c++20
    static std::unordered_map<std::string,
                              std::function<std::unique_ptr<Component>()>>
        &get_factories() {
        static std::unordered_map<std::string,
                                  std::function<std::unique_ptr<Component>()>>
            derivedFactories;
        return derivedFactories;
    }

    // tuple with the types of each serialization function
    using fserial = std::tuple<
        std::function<void(cereal::JSONOutputArchive &, entt::registry const &,
                           entt::entity const)>,
        std::function<void(cereal::PortableBinaryOutputArchive &,
                           entt::registry const &, entt::entity const)>>;

    static std::unordered_map<entt::id_type, fserial> &get_id_mappings() {
        static std::unordered_map<entt::id_type, fserial> mapping;
        return mapping;
    }

    ComponentFactory() {}

public:
    template <typename Derived>
    static bool registerType() {
        // register a factory
        auto &factories = get_factories();
        factories.try_emplace(Derived::name, []() {
            return std::unique_ptr<Component>(new Derived);
        });

        // register all serialization functions for the component
        auto &mapping = get_id_mappings();
        mapping.try_emplace(
            entt::type_info<Derived>::id(),
            [](cereal::JSONOutputArchive &archive, const entt::registry &r,
               const entt::entity e) { r.get<Derived>(e).save(archive); },
            [](cereal::PortableBinaryOutputArchive &archive,
               const entt::registry &r,
               const entt::entity e) { r.get<Derived>(e).save(archive); });
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

    static void save(cereal::JSONOutputArchive &archive,
                          const entt::id_type id,
                          const entt::registry &registry,
                          const entt::entity entity) {
        auto &mapping = get_id_mappings();
        auto it = mapping.find(id);
        if (it != mapping.end())
            std::get<0>(it->second)(archive, registry, entity);
    }

    static void save(cereal::PortableBinaryOutputArchive &archive,
                          const entt::id_type id,
                          const entt::registry &registry,
                          const entt::entity entity) {
        auto &mapping = get_id_mappings();
        auto it = mapping.find(id);
        if (it != mapping.end())
            std::get<1>(it->second)(archive, registry, entity);
    }
};

template <typename Derived>
bool ComponentRegistry<Derived>::dummy =
    ComponentFactory::registerType<Derived>();

