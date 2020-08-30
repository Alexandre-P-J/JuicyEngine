#include "ResourceManager.h"
#include <VertexLayouts.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <assimp/Importer.hpp>
#include <cassert>
#include <fstream>
#include <optional>
#include <type_traits>
#include "nlohmann/json_fwd.hpp"

namespace JuicyEngine {

Resource<ResourceManager::mesh> ResourceManager::get_default_mesh() {
    if (!default_mesh)
        init_default_mesh();
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

    default_mesh =
        Resource<mesh>(mesh_ptr, [vertices, indices](auto ptr) {
            // deallocate mesh data
            delete[] vertices;
            delete[] indices;
            // destroy data handles
            bgfx::destroy(ptr->first);
            bgfx::destroy(ptr->second);
            delete ptr;
        });
}

Resource<bgfx::ShaderHandle> ResourceManager::load_shader(
    const std::filesystem::path& path) {
    if (auto resource = lookup<bgfx::ShaderHandle>(path.string())) {
        return resource;
    }
    auto aux = path;
    auto sh_path = std::filesystem::path(resources_dir) /
                   aux.remove_filename() / get_shaders_subdir() /
                   aux.filename();
    std::ifstream file(sh_path, std::ifstream::binary);
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
            auto key = path.string();
            auto& value = resources[key];
            auto resource = Resource<bgfx::ShaderHandle>(
                new bgfx::ShaderHandle(bgfx::createShader(mem)),
                [this, key](auto sh) {
                    this->resources.erase(key);  // by key since iter may be UB
                    bgfx::destroy(*sh);
                    delete sh;
                });
            value = resource;
            return resource;
        }
        else {
            delete[] buffer;
            spdlog::warn("Error reading " + std::string(sh_path));
            return Resource<bgfx::ShaderHandle>();
        }
    }
    spdlog::warn("Error opening " + std::string(sh_path));
    return Resource<bgfx::ShaderHandle>();
}

Resource<bgfx::ProgramHandle, bgfx::ShaderHandle, bgfx::ShaderHandle>
ResourceManager::load_program(std::filesystem::path const& vs_path,
                              std::filesystem::path const& fs_path) {
    std::string managed_key = vs_path.string() + ":" + fs_path.string();
    if (auto vs_fs =
            lookup<bgfx::ProgramHandle, bgfx::ShaderHandle, bgfx::ShaderHandle>(
                managed_key)) {
        return vs_fs;
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
    }
    spdlog::warn("Shader " + fs_path.string() + " could not be loaded.");
    return Resource<bgfx::ProgramHandle, bgfx::ShaderHandle,
                    bgfx::ShaderHandle>();
}

Resource<nlohmann::json> ResourceManager::load_json(
    std::filesystem::path const& json_path) {
    auto rpath = std::filesystem::path(resources_dir) / json_path;
    auto managed_key = rpath.string();
    if (auto resource = lookup<nlohmann::json>(managed_key)) {
        spdlog::info("FOUND");
        return resource;
    }
    try {
        std::ifstream file(rpath);
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
    } catch (const std::exception&) {
        spdlog::warn("JSON " + managed_key + " could not be loaded.");
        return Resource<nlohmann::json>();
    }
}

// std::shared_ptr<std::string> JuicyEngine::ResourceManager::load_text(
// const std::string& path) {
// auto stored = resources.find(path);
// if (stored != resources.end()) {
// try {
// auto wp = std::get<std::weak_ptr<std::string>>(stored->second);
// if (auto sp = wp.lock()) {
// return sp;
//}
//} catch (const std::bad_variant_access&) {
// spdlog::warn("Tried to load in-memory resource " + path +
//", but it wasn't a text! Reloading from disk");
//}
// resources.erase(stored);
//}
// std::filesystem::path tx_path =
// std::filesystem::path(data_dir) / std::filesystem::path(path);
// std::ifstream file(tx_path);
// if (file) {
// std::stringstream buffer;
// buffer << file.rdbuf();
// file.close();
// auto result = std::make_shared<std::string>(buffer.str());
// resources[path] = std::variant<std::weak_ptr<std::string>>(result);
// return result;
//}
// spdlog::warn(std::string(tx_path) + " couldn't be loaded.");
// return std::shared_ptr<std::string>(nullptr);
//}

std::filesystem::path JuicyEngine::ResourceManager::get_shaders_subdir() {
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
    return std::filesystem::path(shader_resources_subdir) /
           std::filesystem::path(subdir);
}

}  // namespace JuicyEngine
