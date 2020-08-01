#pragma once
#include <Components/Component.h>

struct SerializationComponent
    : public ComponentRegistry<SerializationComponent> {
    static constexpr const char* name = "Serialization";
    std::string entity_name = "Entity";
};
