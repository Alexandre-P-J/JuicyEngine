#pragma once
#include <spdlog/spdlog.h>
#include <entt/entt.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

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


