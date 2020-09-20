#pragma once
#include <Resource.h>
#include <bgfx/bgfx.h>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <functional>
#include <nlohmann/json.hpp>
#include <optional>
#include <unordered_map>
#include <variant>

namespace JuicyEngine {

class ResourceManager {
    friend class Engine;
    // type definitions
    using mesh = std::pair<bgfx::VertexBufferHandle, bgfx::IndexBufferHandle>;
    using hashmap_value = std::variant<
        ResourceHandle<std::string>, ResourceHandle<nlohmann::json>,
        ResourceHandle<bgfx::ProgramHandle, bgfx::ShaderHandle,
                       bgfx::ShaderHandle>,
        ResourceHandle<bgfx::ShaderHandle>, ResourceHandle<bgfx::TextureHandle>,
        ResourceHandle<mesh>, std::monostate>;
    // resource map
    std::unordered_map<std::string, hashmap_value> resources;
    // path settings
    static constexpr const char* resources_dir = "data";
    static constexpr const char* shader_resources_subdir = "shaders";
    // default mesh: plane
    Resource<ResourceManager::mesh> default_mesh;

    ResourceManager() = default;

    void init_default_mesh();
    // checks if the given path exists and is relative to resources_dir.
    // returns a canonical path on success.
    std::optional<std::filesystem::path> path_check(
        std::filesystem::path const& path) const noexcept;

    // composes a new path with shader_resources_subdir and platform specific
    // shader subdir then checks if the new path exists and is inside
    // resources_dir and returns its canonical version on success.
    std::optional<std::filesystem::path> shader_real_path(
        std::filesystem::path const& path) const noexcept;

    Resource<bgfx::ShaderHandle> load_shader(
        const std::filesystem::path& shader);

    template <class ResourceType, class... DependenciesTypes>
    Resource<ResourceType, DependenciesTypes...> lookup(
        std::string const& key) {
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

public:
    Resource<mesh> get_default_mesh();

    Resource<bgfx::ProgramHandle, bgfx::ShaderHandle, bgfx::ShaderHandle>
    load_program(std::filesystem::path const& vs_path,
                 std::filesystem::path const& fs_path);

    Resource<nlohmann::json> load_json(std::filesystem::path const& json_path);

    Resource<std::string> load_text(std::filesystem::path const& text_path);
};

}  // namespace JuicyEngine
