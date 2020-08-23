#pragma once
#include <Components/Component.h>
#include <Engine.h>
#include <VertexLayouts.h>
#include <bgfx/bgfx.h>
#include <vector>

struct RenderComponent : public ComponentRegistry<RenderComponent> {
    static constexpr const char* name = "Render";
    bgfx::VertexBufferHandle vertices;
    bgfx::IndexBufferHandle indexes;
    JuicyEngine::Resource<bgfx::ProgramHandle, bgfx::ShaderHandle,
                          bgfx::ShaderHandle>
        shader;
    std::vector<bgfx::UniformHandle> uniforms;

    // template <class Archive>
    // void save(Archive& ar) const {
    // ar(cereal::make_nvp("Render", 666));
    //}
    // template <class Archive>
    // void load(Archive& ar);
    void save(nlohmann::json& json) const override { json = 3; }
    void load(nlohmann::json const& json) override {
        //static PosColorVertex vert[] = {{25.0f, 25.0f, 5.0f, 0xff0000ff},
                                        //{25.0f, -25.0f, 0.0f, 0xff00ffff},
                                        //{-25.0f, -25.0f, 0.0f, 0xffff0000},
                                        //{-25.0f, 25.0f, 0.0f, 0xff00ff00}};
        //vertices = bgfx::createVertexBuffer(bgfx::makeRef(vert, sizeof(vert)),
                                            //PosColorVertex::ms_layout);

        //static const uint16_t indices[] = {0, 3, 2, 2, 1, 0};
        //indexes =
            //bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(indices)));
        //shader = JuicyEngine::Engine::instance()
                     //.get_resource_manager()
                     //->load_program("basic/v_simple.bin", "basic/f_simple.bin");
        //if (!shader) {
            //spdlog::warn("BAD SHADER");
        //}
    }
    ~RenderComponent() {
        spdlog::warn("Render Component DESTROY");
    }
};
