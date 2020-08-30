#pragma once
#include <Resource.h>
#include <bgfx/bgfx.h>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <functional>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <variant>

namespace JuicyEngine {

class ResourceManager {
    friend class Engine;
    // type definitions
    using mesh = std::pair<bgfx::VertexBufferHandle, bgfx::IndexBufferHandle>;
    using hashmap_value =
        std::variant<ResourceHandle<nlohmann::json>,
                     ResourceHandle<bgfx::ProgramHandle, bgfx::ShaderHandle,
                                    bgfx::ShaderHandle>,
                     ResourceHandle<bgfx::ShaderHandle>,
                     ResourceHandle<bgfx::TextureHandle>, ResourceHandle<mesh>,
                     std::monostate>;
    // resource map
    std::unordered_map<std::string, hashmap_value> resources;
    // path settings
    static constexpr const char* resources_dir = "data";
    static constexpr const char* shader_resources_subdir = "shaders";
    // default mesh: plane
    Resource<ResourceManager::mesh> default_mesh;
    
    ResourceManager() = default;
    void init_default_mesh();
    std::filesystem::path
    get_shaders_subdir();  // all shaders match this partial path

    template <class ResourceType, class... DependenciesTypes>
    Resource<ResourceType, DependenciesTypes...> lookup(
        std::string const &key) {
        if (auto it = resources.find(key); it != resources.end()) {
            if (auto rh_ptr = std::get_if<
                    ResourceHandle<ResourceType, DependenciesTypes...>>(
                    &(it->second))) {
                auto rh = *rh_ptr;
                return rh;  // implicit conversion to Resource
            }
        }
        return Resource<ResourceType, DependenciesTypes...>();
    }

    Resource<bgfx::ShaderHandle> load_shader(
        const std::filesystem::path &shader);
public:
    Resource<mesh> get_default_mesh();
    Resource<bgfx::ProgramHandle, bgfx::ShaderHandle, bgfx::ShaderHandle>
    load_program(std::filesystem::path const &vs_path,
                 std::filesystem::path const &fs_path);
    Resource<nlohmann::json> load_json(std::filesystem::path const &json_path);
    // std::shared_ptr<std::string> load_text(const std::string &path);
};

}  // namespace JuicyEngine
