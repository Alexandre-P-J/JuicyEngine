#pragma once
#include <Components/Component.h>
#include <spdlog/spdlog.h>

struct SerializationComponent
    : public ComponentRegistry<SerializationComponent> {
    static constexpr const char* name = "Serialization";

    void save(nlohmann::json& json) const override {}
    void load(nlohmann::json const& json) override {}
};
