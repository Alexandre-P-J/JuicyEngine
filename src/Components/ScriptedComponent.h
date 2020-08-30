#pragma once
#include <Components/Component.h>
#include <spdlog/spdlog.h>
#include <filesystem>
#include <sol/sol.hpp>
#include <type_traits>

struct ScriptedComponent : public ComponentRegistry<ScriptedComponent> {
    static constexpr const char* name = "Scripted";

    std::unordered_map<std::string, sol::lua_table> components;

    template <class... str>
    bool has_all(str const&... values) {
        static_assert(std::is_same<str..., std::string const&>::value);
        return (components.count(values) && ...);
    }

    void save(nlohmann::json& json) const override {}
    void load(nlohmann::json const& json) override {}
};
