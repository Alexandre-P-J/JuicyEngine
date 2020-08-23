#include <RenderManager.h>

#include <WindowManager.h>
#include <spdlog/spdlog.h>

namespace JuicyEngine {

RenderManager::RenderManager() {
    if (!SDL_WasInit(SDL_INIT_VIDEO)) {
        spdlog::critical(
            "SDL_VIDEO not initialized before RenderSystem construction");
    }

    bgfx::renderFrame();  // Render empty frame
    bgfx::Init init;
    init.type = bgfx::RendererType::Enum::Count;
    init.resolution.width = width;
    init.resolution.height = height;
    init.resolution.reset = BGFX_RESET_VSYNC;
    bgfx::init(init);

    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f,
                       0);
    bgfx::setViewRect(0, 0, 0, width, height);
}

RenderManager::~RenderManager() { bgfx::shutdown(); }

}  // namespace JuicyEngine
