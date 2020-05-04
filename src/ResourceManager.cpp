#include <ResourceManager.h>
#include <spdlog/spdlog.h>
namespace fs = std::filesystem;


std::shared_ptr<ResourceManager> ResourceManager::get_instance() {
    static std::shared_ptr<ResourceManager> ptr(new ResourceManager);
    return ptr;
}



std::optional<bgfx::ShaderHandle>
ResourceManager::load_shader(const std::string& path) {
    fs::path aux = fs::path(path);
    fs::path sh_path = fs::path(data_dir) / aux.remove_filename() /
                       get_shaders_subdir() / aux.filename();
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
            bgfx::ShaderHandle handle = bgfx::createShader(mem);
            bgfx::setName(handle, sh_path.c_str());
            return handle;
        } else {
            delete[] buffer;
            spdlog::warn("Error reading " + std::string(sh_path));
            return std::nullopt; // file read problem
        }
    }
    spdlog::warn("Error opening " + std::string(sh_path));
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
ResourceManager::load_shader_program(const std::string& vs_path,
                                     const std::string& fs_path) {
    auto vs = load_shader(vs_path);
    if (vs) {
        auto fs = load_shader(fs_path);
        if (fs) {
            bgfx::ProgramHandle shader_program =
                bgfx::createProgram(vs.value(), fs.value());
            return shader_program;
        } else {
            spdlog::warn("Fragment shader " + fs_path +
                         " could not be loaded.");
        }
    } else {
        spdlog::warn("Vertex shader " + vs_path + " could not be loaded.");
    }
    return std::nullopt;
}



std::shared_ptr<std::string> ResourceManager::load_text(const std::string& path) {
    auto stored = resources.find(path);
    if (stored != resources.end()) {
        try {
            auto wp = std::get<std::weak_ptr<std::string>>(stored->second);
            if (auto sp = wp.lock()) {
                return sp;
            }
        }
        catch (const std::bad_variant_access&) {
            spdlog::warn("Tried to load in-memory resource "+path+", but it wasn't a text! Reloading from disk");
        }
        resources.erase(stored);
    }
    fs::path tx_path = fs::path(data_dir) / fs::path(path);
    std::ifstream file(tx_path);
    if (file) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        auto result = std::make_shared<std::string>(buffer.str());
        resources[path] = std::variant<std::weak_ptr<std::string>>(result);
        return result;
    }
    spdlog::warn(std::string(tx_path)+" couldn't be loaded.");
    return std::shared_ptr<std::string>(nullptr);
}