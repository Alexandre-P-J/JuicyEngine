#pragma once
#include <Resource.h>
#include <bgfx/bgfx.h>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <functional>
#include <unordered_map>
#include <variant>

#ifndef SHADERS_PARTIAL_PATH
    #define SHADERS_PARTIAL_PATH "shaders"  // fallback
#endif
#ifndef RESOURCES_FOLDER
    #define RESOURCES_FOLDER "data"  // fallback
#endif

namespace JuicyEngine {

class ResourceManager {
    friend class Engine;
    using mesh = std::tuple<bgfx::VertexBufferHandle, bgfx::IndexBufferHandle>;
    using hashmap_value =
        std::variant<ResourceHandle<bgfx::ProgramHandle, bgfx::ShaderHandle,
                                    bgfx::ShaderHandle>,
                     ResourceHandle<bgfx::ShaderHandle>,
                     ResourceHandle<bgfx::TextureHandle>, ResourceHandle<mesh>,
                     std::monostate>;
    std::unordered_map<std::string, hashmap_value> resources;
    static constexpr const char *data_dir = RESOURCES_FOLDER;
    static constexpr const char *shaders_partial_path = SHADERS_PARTIAL_PATH;

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
    ResourceManager();
    Resource<ResourceManager::mesh> default_sprite;
    Resource<bgfx::ProgramHandle, bgfx::ShaderHandle, bgfx::ShaderHandle>
    load_program(std::filesystem::path const &vs_path,
                 std::filesystem::path const &fs_path);
    
    Resource<mesh> default_sprite_quad();
    // std::shared_ptr<std::string> load_text(const std::string &path);
};

}  // namespace JuicyEngine
