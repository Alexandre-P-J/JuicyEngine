#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <Utils/ECS.h>
#include <spdlog/spdlog.h>
#include <iostream>


namespace JuicyEngine {

class System {
public:
    virtual ~System() = default;
    virtual std::string get_name() const = 0;
    virtual void update(Registry& scene) = 0;
};

template <typename Derived>
class SystemRegistry : public System {
    static bool dummy;

    template <typename T, T>
    struct dummy_value {};
    typedef dummy_value<bool &, dummy>
        dummy_value_type;  // force dummy to get evaluated

public:
    std::string get_name() const override { return Derived::name; }
};

class SystemFactory {
    static std::unordered_map<std::string, std::unique_ptr<System> (*)()>
        &get_factories() {
        static std::unordered_map<std::string, std::unique_ptr<System> (*)()>
            derivedFactories;
        return derivedFactories;
    }

    SystemFactory() = default;

public:
    template <typename Derived>
    static bool registerType() {
        std::cout << "REGISTER System" << std::endl;
        auto &factories = get_factories();
        factories.try_emplace(Derived::name, []() {
            return std::unique_ptr<System>(new Derived);
        });
        return true;
    }

    static std::unique_ptr<System> create(const std::string &name) {
        auto &factories = get_factories();
        auto it = factories.find(name);
        if (it != factories.end())
            return it->second();
        else
            return std::unique_ptr<System>();  // invalid request
    }

    static std::vector<std::unique_ptr<System>> create_all() {
        auto& factories = get_factories();
        std::vector<std::unique_ptr<System>> result;
        result.reserve(factories.size());
        for (auto& system : get_factories()) {
            result.push_back(system.second());
        }
        return result;
    }
};

template <typename Derived>
bool SystemRegistry<Derived>::dummy = SystemFactory::registerType<Derived>();

}
