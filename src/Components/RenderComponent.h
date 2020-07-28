#pragma once
#include <bgfx/bgfx.h>
#include <vector>

struct RenderComponent {
    bgfx::VertexBufferHandle vertices;
    bgfx::IndexBufferHandle indexes;
    bgfx::ProgramHandle shader;
    std::vector<bgfx::UniformHandle> uniforms;
    
    template<class Archive>
    void serialize(Archive & ar) {
        ar(666);
    }

};
