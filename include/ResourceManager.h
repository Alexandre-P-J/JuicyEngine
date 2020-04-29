#pragma once
#include <bgfx/bgfx.h>
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>

#ifndef SHADERS_PARTIAL_PATH
#define SHADERS_PARTIAL_PATH "shaders"
#endif
#ifndef RESOURCES_FOLDER
#define RESOURCES_FOLDER "data"
#endif


// WIP: Shader reuse, dont load from disk if its in memory
// WIP: More helper functions, to allow uniform link and getting the shader
// program from different arguments

class ResourceManager {
    const std::string data_dir = RESOURCES_FOLDER;
    const std::string shaders_partial_path = SHADERS_PARTIAL_PATH;
    std::filesystem::path
    get_shaders_subdir(); // all shaders match this partial path
    ResourceManager(){};

public:
    static std::shared_ptr<ResourceManager> get_instance();
    std::optional<bgfx::ShaderHandle> loadShader(const std::string& name);
    std::optional<bgfx::ProgramHandle>
    loadShaderProgram(const std::string& vs_name, const std::string& fs_name);
    std::optional<std::string> load_text(const std::string& path);
};