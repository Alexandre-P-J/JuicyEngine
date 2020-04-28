#include <ResourceManager.h>
#include <spdlog/spdlog.h>

const std::string ResourceManager::shader_dir = "shaders";



std::shared_ptr<ResourceManager> ResourceManager::get_instance() {
    static std::shared_ptr<ResourceManager> ptr(new ResourceManager);
    return ptr;
}



std::optional<bgfx::ShaderHandle> ResourceManager::loadShader(const std::string& name) {
    auto path = get_shaders_path() + name;
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
            spdlog::warn("Error reading " + path);
            return std::nullopt;// file read problem
        }
    }
    spdlog::warn("Error opening " + path);
    return std::nullopt; // file open problem
}



std::string ResourceManager::get_shaders_path() {
    std::string subdir;
    switch (bgfx::getRendererType()) {
        case bgfx::RendererType::Noop:
        case bgfx::RendererType::Direct3D9:
            subdir = "/dx9/";
            break;
        case bgfx::RendererType::Direct3D11:
        case bgfx::RendererType::Direct3D12:
            subdir = "/dx11/";
            break;
        case bgfx::RendererType::Gnm:
            break;
        case bgfx::RendererType::Metal:
            subdir = "/metal/";
            break;
        case bgfx::RendererType::OpenGL:
            subdir = "/glsl/";
            break;
        case bgfx::RendererType::OpenGLES:
            subdir = "/essl/";
            break;
        case bgfx::RendererType::Vulkan:
            subdir = "/spirv/";
            break;
        case bgfx::RendererType::Count:
            break;
    }
    return shader_dir + subdir;
}



std::optional<bgfx::ProgramHandle> ResourceManager::loadShaderProgram(const std::string& vs_name,
                                                               const std::string& fs_name) {
    auto vs = loadShader(vs_name);
    if (vs) {
        auto fs = loadShader(fs_name);
        if (fs) {
            bgfx::ProgramHandle shader_program = bgfx::createProgram(vs.value(), fs.value());
            return shader_program;
        } else {
            spdlog::warn("Fragment shader " + fs_name + " could not be loaded.");
        }
    } else {
        spdlog::warn("Vertex shader " + vs_name + " could not be loaded.");
    }
    return std::nullopt;
}