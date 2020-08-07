#pragma once
#include <Components/Component.h>
#include <bgfx/bgfx.h>
#include <vector>

struct RenderComponent : public ComponentRegistry<RenderComponent> {
    static constexpr const char* name = "Render";
    bgfx::VertexBufferHandle vertices;
    bgfx::IndexBufferHandle indexes;
    bgfx::ProgramHandle shader;
    std::vector<bgfx::UniformHandle> uniforms;

    template <class Archive>
    void save(Archive& ar) const {
        ar(cereal::make_nvp("Render", 666));
    }
    template <class Archive>
    void load(Archive& ar);

};
