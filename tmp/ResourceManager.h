#pragma once
#include <bgfx/bgfx.h>
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <unordered_map>
#include <variant>

#ifndef SHADERS_PARTIAL_PATH
    #define SHADERS_PARTIAL_PATH "shaders"  // fallback
#endif
#ifndef RESOURCES_FOLDER
    #define RESOURCES_FOLDER "data"  // fallback
#endif

template <class T>
using wp = std::weak_ptr<T>;
using ResourceType = std::variant<wp<std::string> >;
// WIP: Shader reuse, dont load from disk if its in memory
// WIP: More helper functions, to allow uniform link and getting the shader
// program from different arguments

class ResourceManager {
    std::unordered_map<std::string, ResourceType>
        resources;  // <path,weak_ptr<text>

    const std::string data_dir = RESOURCES_FOLDER;
    const std::string shaders_partial_path = SHADERS_PARTIAL_PATH;
    std::filesystem::path
    get_shaders_subdir();  // all shaders match this partial path
    ResourceManager(){};

public:
    static std::shared_ptr<ResourceManager> get_instance();
    std::optional<bgfx::ShaderHandle> load_shader(const std::string& name);
    std::optional<bgfx::ProgramHandle> load_shader_program(
        const std::string& vs_name, const std::string& fs_name);
    std::shared_ptr<std::string> load_text(const std::string& path);
};
