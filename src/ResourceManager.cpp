#include "ResourceManager.h"
#include <VertexLayouts.h>
#include <Logging.h>
#include <algorithm>
#include <cassert>
#include <fstream>
#include <optional>
#include <type_traits>
#include "nlohmann/json_fwd.hpp"

namespace JuicyEngine {

Resource<ResourceManager::mesh> ResourceManager::get_default_mesh() {
    if (!default_mesh) init_default_mesh();
    return default_mesh;
}

void ResourceManager::init_default_mesh() {
    PosVertex* vertices = new PosVertex[4]{{25.0f, 25.0f, 0},
                                           {25.0f, -25.0f, 0},
                                           {-25.0f, -25.0f, 0},
                                           {-25.0f, 25.0f, 0}};
    auto verticesh = bgfx::createVertexBuffer(
        bgfx::makeRef(vertices, sizeof(PosVertex) * 4), PosVertex::ms_layout);

    uint16_t* indices = new uint16_t[6]{0, 3, 2, 2, 1, 0};
    auto indicesh =
        bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(uint16_t) * 6));

    auto mesh_ptr = new mesh(verticesh, indicesh);

    default_mesh = Resource<mesh>(mesh_ptr, [vertices, indices](auto ptr) {
        // deallocate mesh data
        delete[] vertices;
        delete[] indices;
        // destroy data handles
        bgfx::destroy(ptr->first);
        bgfx::destroy(ptr->second);
        delete ptr;
    });
}

std::optional<std::filesystem::path> ResourceManager::path_check(
    std::filesystem::path const& path) const noexcept {
    try {
        auto root = std::filesystem::path(resources_dir);
        auto canonical = std::filesystem::canonical(root / path);
        auto relative = root / std::filesystem::relative(canonical, root);
        auto [rootEnd, nothing] =
            std::mismatch(root.begin(), root.end(), relative.begin());
        if (rootEnd == root.end()) return canonical;
        return std::nullopt;
    } catch (std::exception const&) {
        return std::nullopt;
    }
}

std::optional<std::filesystem::path> ResourceManager::shader_real_path(
    std::filesystem::path const& path) const noexcept {
    std::string subdir;
    switch (bgfx::getRendererType()) {
        case bgfx::RendererType::Noop:
        case bgfx::RendererType::Direct3D9:
            subdir = "dx9";
            break;
        case bgfx::RendererType::Direct3D11:
        case bgfx::RendererType::Direct3D12:
            subdir = "dx11";
            break;
        case bgfx::RendererType::Gnm:
            break;
        case bgfx::RendererType::Metal:
            subdir = "metal";
            break;
        case bgfx::RendererType::OpenGL:
            subdir = "glsl";
            break;
        case bgfx::RendererType::OpenGLES:
            subdir = "essl";
            break;
        case bgfx::RendererType::Vulkan:
            subdir = "spirv";
            break;
        default:
            break;
    }
    auto composed =
        path.parent_path() / shader_resources_subdir / subdir / path.filename();
    return path_check(composed);
}

Resource<bgfx::ShaderHandle> ResourceManager::load_shader(
    const std::filesystem::path& path) {
    auto real_path = shader_real_path(path);

    if (!real_path) {
        Logging::Engine(Level::warning,
                        "Cannot read " + path.string() + " path is invalid");
        return Resource<bgfx::ShaderHandle>();
    }

    auto managed_key =
        std::filesystem::relative(*real_path, resources_dir).string();

    if (auto shader = lookup<bgfx::ShaderHandle>(managed_key)) {
        return shader;
    }
    std::ifstream file(*real_path, std::ifstream::binary);
    if (file) {
        file.seekg(0, file.end);
        size_t length = file.tellg();
        file.seekg(0, file.beg);

        char* buffer = new char[length];
        file.read(buffer, length);
        if (file) {
            const bgfx::Memory* mem = bgfx::copy(buffer, length + 1);
            delete[] buffer;
            file.close();
            mem->data[mem->size - 1] = '\0';
            auto& value = resources[managed_key];
            auto resource = Resource<bgfx::ShaderHandle>(
                new bgfx::ShaderHandle(bgfx::createShader(mem)),
                [this, managed_key](auto sh) {
                    this->resources.erase(
                        managed_key);  // by key since iter may be UB
                    bgfx::destroy(*sh);
                    delete sh;
                });
            value = resource;
            return resource;
        }
        else {
            delete[] buffer;
            Logging::Engine(Level::warning,
                            "Error reading " + real_path->string());
            return Resource<bgfx::ShaderHandle>();
        }
    }
    Logging::Engine(Level::warning, "Error opening " + real_path->string());
    return Resource<bgfx::ShaderHandle>();
}

Resource<bgfx::ProgramHandle, bgfx::ShaderHandle, bgfx::ShaderHandle>
ResourceManager::load_program(std::filesystem::path const& vs_path,
                              std::filesystem::path const& fs_path) {
    auto vs_real_path = shader_real_path(vs_path);
    auto fs_real_path = shader_real_path(fs_path);

    if (!(vs_real_path && fs_real_path)) {
        Logging::Engine(Level::warning, "Invalid path on " + vs_path.string() +
                                            " or " + fs_path.string());
        return Resource<bgfx::ProgramHandle, bgfx::ShaderHandle,
                        bgfx::ShaderHandle>();
    }
    auto vs_real_relative_path =
        std::filesystem::relative(*vs_real_path, resources_dir);
    auto fs_real_relative_path =
        std::filesystem::relative(*fs_real_path, resources_dir);
    auto managed_key =
        vs_real_relative_path.string() + ":" + vs_real_relative_path.string();
    if (auto program =
            lookup<bgfx::ProgramHandle, bgfx::ShaderHandle, bgfx::ShaderHandle>(
                managed_key)) {
        return program;
    }
    if (auto vs = load_shader(vs_path)) {
        if (auto fs = load_shader(fs_path)) {
            auto& value = resources[managed_key];
            auto resource = Resource<bgfx::ProgramHandle, bgfx::ShaderHandle,
                                     bgfx::ShaderHandle>(
                new bgfx::ProgramHandle(bgfx::createProgram(*vs, *fs, false)),
                [this, managed_key](auto p) {
                    // erase by key since iter may be UB
                    this->resources.erase(managed_key);
                    bgfx::destroy(*p);
                    delete p;  // Program auto-destroys the program handler
                },
                vs, fs);
            value = resource;
            return resource;
        }
        Logging::Engine(Level::warning,
                        "Shader " + fs_path.string() + " could not be loaded.");
    }
    Logging::Engine(Level::warning,
                    "Shader " + vs_path.string() + " could not be loaded.");
    return Resource<bgfx::ProgramHandle, bgfx::ShaderHandle,
                    bgfx::ShaderHandle>();
}

Resource<nlohmann::json> ResourceManager::load_json(
    std::filesystem::path const& json_path) {
    auto real_path = path_check(json_path);
    if (!real_path) {
        Logging::Engine(Level::warning, "Cannot read " + json_path.string() +
                                            " path is invalid");
        return Resource<nlohmann::json>();
    }
    auto managed_key =
        std::filesystem::relative(*real_path, resources_dir).string();
    if (auto resource = lookup<nlohmann::json>(managed_key)) {
        return resource;
    }
    try {
        std::ifstream file(*real_path);
        const std::string str((std::istreambuf_iterator<char>(file)),
                              (std::istreambuf_iterator<char>()));
        auto json_ptr = new nlohmann::json(nlohmann::json::parse(str));
        auto& value = resources[managed_key];
        auto resource =
            Resource<nlohmann::json>(json_ptr, [this, managed_key](auto ptr) {
                this->resources.erase(managed_key);
                delete ptr;
            });
        value = resource;
        return resource;
    } catch (std::exception const&) {
        Logging::Engine(Level::warning,
                        "JSON " + managed_key + " could not be loaded.");
        return Resource<nlohmann::json>();
    }
}

Resource<std::string> ResourceManager::load_text(
    std::filesystem::path const& text_path) {
    auto real_path = path_check(text_path);
    if (!real_path) {
        Logging::Engine(Level::warning, "Cannot read " + text_path.string() +
                                            " path is invalid");
        return Resource<std::string>();
    }
    auto managed_key =
        std::filesystem::relative(*real_path, resources_dir).string();
    if (auto resource = lookup<std::string>(managed_key)) {
        return resource;
    }
    try {
        std::ifstream file(*real_path);
        const auto str_ptr =
            new std::string((std::istreambuf_iterator<char>(file)),
                            (std::istreambuf_iterator<char>()));
        auto& value = resources[managed_key];
        auto resource =
            Resource<std::string>(str_ptr, [this, managed_key](auto ptr) {
                this->resources.erase(managed_key);
                delete ptr;
            });
        value = resource;
        return resource;
    } catch (std::exception const&) {
        Logging::Engine(Level::warning,
                        "String " + managed_key + " could not be loaded.");
        return Resource<std::string>();
    }
}

}  // namespace JuicyEngine
