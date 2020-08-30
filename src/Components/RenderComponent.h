#pragma once
#include <Components/Component.h>
#include <Engine.h>
#include <VertexLayouts.h>
#include <bgfx/bgfx.h>
#include <vector>

namespace JuicyEngine {

struct RenderComponent : public ComponentRegistry<RenderComponent> {
    static constexpr const char* name = "Render";

    std::filesystem::path mesh_path;
    Resource<std::pair<bgfx::VertexBufferHandle, bgfx::IndexBufferHandle>>
        mesh;

    std::filesystem::path vs_path;
    std::filesystem::path fs_path;
    Resource<bgfx::ProgramHandle, bgfx::ShaderHandle, bgfx::ShaderHandle>
        shader;

    std::vector<bgfx::UniformHandle> uniforms;

    void save(nlohmann::json& json) const override {
        if (!mesh_path.empty())
            json["mesh"] = mesh_path;
        if (!(vs_path.empty() || fs_path.empty())) {
            json["vertex shader"] = vs_path;
            json["fragment shader"] = fs_path;
        }
    }
    void load(nlohmann::json const& json) override {
        auto r_manager = Engine::instance().get_resource_manager();
        if (json.contains("mesh")) {
            json["mesh"].get_to(mesh_path);
            //mesh = r_manager->load_mesh(mesh_path);
        }
        else
            mesh = r_manager->get_default_mesh();
        if (json.contains("vertex shader") && json.contains("fragment shader")) {
            json["vertex shader"].get_to(vs_path);
            json["fragment shader"].get_to(fs_path);
            shader = r_manager->load_program(vs_path, fs_path); 
        }
    }
    ~RenderComponent() { spdlog::warn("Render Component DESTROY"); }
};

}  // namespace JuicyEngine
