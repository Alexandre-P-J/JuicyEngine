#pragma once
#include <entt/entt.hpp>

// PLATFORM DEFINITIONS: Pending refactor?
#include <bgfx/platform.h>
#if defined(None)  // X11 defines this...
    #undef None
#endif  // defined(None)
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    #define SDL_VIDEO_DRIVER_X11
#elif BX_PLATFORM_WINDOWS
    #define SDL_VIDEO_DRIVER_WINDOWS
#elif BX_PLATFORM_OSX
    #define SDL_VIDEO_DRIVER_COCOA
#elif BX_PLATFORM_STEAMLINK
    #define SDL_VIDEO_DRIVER_VIVANTE
#endif
// BGFX
#include <bgfx/bgfx.h>
#include <bx/bx.h>
// SDL
#define SDL_MAIN_HANDLED
#include <SDL.h>
BX_PRAGMA_DIAGNOSTIC_PUSH()
BX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG("-Wextern-c-compat")
#include <SDL_syswm.h>
BX_PRAGMA_DIAGNOSTIC_POP()
#include <Systems/System.h>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

namespace JuicyEngine {
class RenderSystem : public JuicyEngine::System {
    friend class Engine;
    void update(entt::registry& scene);

    // pending refactor:
    uint16_t w_height = 500;
    uint16_t w_width = 500;
    SDL_Window* window = nullptr;
    int max_z_index = 1;
    int min_z_index = -1;

    void* init_window_handle();
    void destroy_window();

public:
    RenderSystem();
    ~RenderSystem();
    void resize_window(int width, int height);
    void set_current_transform(glm::mat4 transform, int z_index);
    void set_current_buffers(bgfx::VertexBufferHandle v,
                             bgfx::IndexBufferHandle i);
    void submit_current(bgfx::ProgramHandle shader);
};
REGISTER_SYSTEM(RenderSystem)
}

