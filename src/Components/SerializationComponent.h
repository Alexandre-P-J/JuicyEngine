#pragma once
#include <Components/Component.h>
#include <spdlog/spdlog.h>

struct SerializationComponent
    : public ComponentRegistry<SerializationComponent> {
    static constexpr const char* name = "Serialization";
    std::string entity_name = "Entity";

    // template <class Archive>
    // void save(Archive& ar) const {
    // ar(cereal::make_nvp("Serialization", 222));
    //}
    // template <class Archive>
    // void load(Archive& ar);
    void save(nlohmann::json& json) const override {
        json["Name"] = entity_name;
    }

    void load(nlohmann::json const& json) override {}
};
