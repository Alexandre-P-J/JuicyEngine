#pragma once
#include <Components/Component.h>

struct SerializationComponent
    : public ComponentRegistry<SerializationComponent> {
    static constexpr const char* name = "Serialization";
    std::string entity_name = "Entity";


    template <class Archive>
    void save(Archive& ar) const {
        ar(cereal::make_nvp("Serialization", 222));
    }
    template <class Archive>
    void load(Archive& ar);

};
