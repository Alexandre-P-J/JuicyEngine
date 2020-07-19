#include "RenderSystem.h"
#include <Components/LensComponent.h>
#include <Components/RenderComponent.h>
#include <Components/TransformComponent.h>
#include <bx/math.h>
#include <spdlog/spdlog.h>
#include "bgfx/bgfx.h"

JuicyEngine::RenderSystem::RenderSystem() {
    window = SDL_CreateWindow("JuicyEngine", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, w_width, w_height,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!window) {
        spdlog::critical("SDL2 Window creation failed.");
    }
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(window, &wmi)) {
        spdlog::critical("SDL2 window manager info query failed.");
    }

    bgfx::PlatformData pd;
    pd.nwh = init_window_handle();

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    #if USE_WAYLAND
    pd.ndt = wmi.info.wl.display;
    #else
    pd.ndt = wmi.info.x11.display;
    #endif
#elif BX_PLATFORM_OSX
    pd.ndt = NULL;
#elif BX_PLATFORM_WINDOWS
    pd.ndt = NULL;
#endif  // BX_PLATFORM_

    pd.context = NULL;
    pd.backBuffer = NULL;
    pd.backBufferDS = NULL;

    bgfx::setPlatformData(pd);
    bgfx::Init init;
    init.type = bgfx::RendererType::Enum::Count;
    init.resolution.width = w_width;
    init.resolution.height = w_height;
    init.resolution.reset = BGFX_RESET_VSYNC;
    bgfx::renderFrame();  // Render empty frame
    bgfx::init(init);

    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f,
                       0);
    bgfx::setViewRect(0, 0, 0, w_width, w_height);
}

JuicyEngine::RenderSystem::~RenderSystem() {
    bgfx::shutdown();
    destroy_window();
}

void *JuicyEngine::RenderSystem::init_window_handle() {
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!window) {
        spdlog::critical("Window not initialized or invalid.");
    }
    if (!SDL_GetWindowWMInfo(window, &wmi)) {
        spdlog::critical("SDL2 window manager info query failed.");
    }

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    #if USE_WAYLAND
    wl_egl_window *win_impl =
        (wl_egl_window *)SDL_GetWindowData(window, "wl_egl_window");
    if (!win_impl) {
        int width, height;
        SDL_GetWindowSize(window, &width, &height);
        struct wl_surface *surface = wmi.info.wl.surface;
        if (!surface) return nullptr;
        win_impl = wl_egl_window_create(surface, width, height);
        SDL_SetWindowData(window, "wl_egl_window", win_impl);
    }
    return (void *)(uintptr_t)win_impl;
    #else
    return (void *)wmi.info.x11.window;
    #endif
#elif BX_PLATFORM_OSX
    return wmi.info.cocoa.window;
#elif BX_PLATFORM_WINDOWS
    return wmi.info.win.window;
#endif  // BX_PLATFORM_
}

void JuicyEngine::RenderSystem::destroy_window() {
    if (!window) {
        spdlog::critical("SDL2 window destroy attempt failed, window is null.");
    }
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    #if USE_WAYLAND
    wl_egl_window *win_impl =
        (wl_egl_window *)SDL_GetWindowData(window, "wl_egl_window");
    if (win_impl) {
        SDL_SetWindowData(window, "wl_egl_window", nullptr);
        wl_egl_window_destroy(win_impl);
    }
    #endif
#endif
    SDL_DestroyWindow(window);
    window = nullptr;
}

void JuicyEngine::RenderSystem::update(entt::registry &scene) {
    auto cameras = scene.view<LensComponent, TransformComponent>();
    // WIP, for now just select an arbitrary camera
    if (auto camera = cameras.front(); scene.valid(camera)) {
        auto const &trans_c = cameras.get<TransformComponent>(camera);
        auto const &lens_c = cameras.get<LensComponent>(camera);

        bgfx::setViewTransform(0, &trans_c.transform[0][0], &lens_c.projection[0][0]);
    }

    auto objects = scene.view<RenderComponent, TransformComponent>();
    for (auto entity : objects) {
        auto &render_data = objects.get<RenderComponent>(entity);
        auto &transform_data = objects.get<TransformComponent>(entity);
        bgfx::setTransform(&transform_data.transform[0][0]);
        bgfx::setVertexBuffer(0, render_data.vertices);
        bgfx::setIndexBuffer(render_data.indexes);
        // Uniforms [WIP]
        bgfx::setState(BGFX_STATE_DEFAULT);
        bgfx::submit(0, render_data.shader);
    }
    bgfx::frame();
}

void JuicyEngine::RenderSystem::resize_window(int width, int height) {
    if (!window) {
        spdlog::critical("Cannot resize window, window is null.");
        return;
    }
    if (width <= 0 || height <= 0) {
        spdlog::critical("Cannot resize window to negative size.");
        return;
    }
    SDL_SetWindowSize(window, width, height);
    bgfx::setViewRect(0, 0, 0, width, height);
}

void JuicyEngine::RenderSystem::submit_current(bgfx::ProgramHandle shader) {
    bgfx::setState(BGFX_STATE_DEFAULT);
    bgfx::submit(0, shader);
}
