#pragma once
#include <bgfx/bgfx.h>
#include <fstream>
#include <optional>
#include <memory>

// WIP: Shader reuse, dont load from disk if its in memory
// WIP: More helper functions, to allow uniform link and getting the shader program from different arguments

class ResourceManager {
    const static std::string shader_dir;
    std::string get_shaders_path();
    ResourceManager() {};

public:
    static std::shared_ptr<ResourceManager> get_instance();
    std::optional<bgfx::ShaderHandle> loadShader(const std::string& name);
    std::optional<bgfx::ProgramHandle> loadShaderProgram(const std::string& vs_name, const std::string& fs_name);
};