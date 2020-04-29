#include <ResourceManager.h>
#include <spdlog/spdlog.h>
namespace fs = std::filesystem;


std::shared_ptr<ResourceManager> ResourceManager::get_instance() {
    static std::shared_ptr<ResourceManager> ptr(new ResourceManager);
    return ptr;
}



std::optional<bgfx::ShaderHandle>
ResourceManager::loadShader(const std::string& relative_path) {
    fs::path aux = fs::path(relative_path);
    fs::path path = fs::path(data_dir) / aux.remove_filename() /
                    get_shaders_subdir() / aux.filename();
    std::ifstream file(path, std::ifstream::binary);
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
            bgfx::ShaderHandle handle = bgfx::createShader(mem);
            bgfx::setName(handle, path.c_str());
            return handle;
        } else {
            delete[] buffer;
            spdlog::warn("Error reading " + std::string(path));
            return std::nullopt; // file read problem
        }
    }
    spdlog::warn("Error opening " + std::string(path));
    return std::nullopt; // file open problem
}



std::filesystem::path ResourceManager::get_shaders_subdir() {
    std::filesystem::path subdir;
    switch (bgfx::getRendererType()) {
        case bgfx::RendererType::Noop:
        case bgfx::RendererType::Direct3D9:
            subdir = fs::path("dx9");
            break;
        case bgfx::RendererType::Direct3D11:
        case bgfx::RendererType::Direct3D12:
            subdir = fs::path("dx11");
            break;
        case bgfx::RendererType::Gnm:
            break;
        case bgfx::RendererType::Metal:
            subdir = fs::path("metal");
            break;
        case bgfx::RendererType::OpenGL:
            subdir = fs::path("glsl");
            break;
        case bgfx::RendererType::OpenGLES:
            subdir = fs::path("essl");
            break;
        case bgfx::RendererType::Vulkan:
            subdir = fs::path("spirv");
            break;
        case bgfx::RendererType::Count:
            break;
    }
    return fs::path(shaders_partial_path) / subdir;
}



std::optional<bgfx::ProgramHandle>
ResourceManager::loadShaderProgram(const std::string& vs_name,
                                   const std::string& fs_name) {
    auto vs = loadShader(vs_name);
    if (vs) {
        auto fs = loadShader(fs_name);
        if (fs) {
            bgfx::ProgramHandle shader_program =
                bgfx::createProgram(vs.value(), fs.value());
            return shader_program;
        } else {
            spdlog::warn("Fragment shader " + fs_name +
                         " could not be loaded.");
        }
    } else {
        spdlog::warn("Vertex shader " + vs_name + " could not be loaded.");
    }
    return std::nullopt;
}