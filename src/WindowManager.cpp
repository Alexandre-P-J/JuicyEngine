#include <WindowManager.h>

#include <Engine.h>
#include <spdlog/spdlog.h>

JuicyEngine::WindowManager::WindowManager() {
    if (!SDL_WasInit(SDL_INIT_VIDEO)) {
        Logging::Engine(
            Level::critical,
            "SDL_VIDEO not initialized before WindowManager construction");
    }
    window = SDL_CreateWindow("JuicyEngine", x, y, width, height, window_flags);
    if (!window) {
        Logging::Engine(Level::critical, "SDL2 Window creation failed.");
    }
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(window, &wmi)) {
        Logging::Engine(Level::critical,
                        "SDL2 window manager info query failed.");
    }
    bgfx::PlatformData pd;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    #if USE_WAYLAND
    wl_egl_window *win_impl =
        (wl_egl_window *)SDL_GetWindowData(window, "wl_egl_window");
    if (!win_impl) {
        int width, height;
        SDL_GetWindowSize(window, &width, &height);
        struct wl_surface *surface = wmi.info.wl.surface;
        if (!surface) pd.nwh = nullptr;
        win_impl = wl_egl_window_create(surface, width, height);
        SDL_SetWindowData(window, "wl_egl_window", win_impl);
    }
    pd.nwh = (void *)(uintptr_t)win_impl;
    pd.ndt = wmi.info.wl.display;
    #else
    pd.nwh = (void *)wmi.info.x11.window;
    pd.ndt = wmi.info.x11.display;
    #endif
#elif BX_PLATFORM_OSX
    pd.nwh = wmi.info.cocoa.window;
    pd.ndt = NULL;
#elif BX_PLATFORM_WINDOWS
    pd.nwh = wmi.info.win.window;
    pd.ndt = NULL;
#endif

    pd.context = NULL;
    pd.backBuffer = NULL;
    pd.backBufferDS = NULL;
    bgfx::setPlatformData(pd);
}

JuicyEngine::WindowManager::~WindowManager() {
    if (!window) {
        return;
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
}

void JuicyEngine::WindowManager::process(WindowEvent event) {
    switch (event) {
        case WindowEvent::close:
            Engine::instance().exit();
            break;
        default:
            break;
    }
}

void JuicyEngine::WindowManager::resize(uint16_t width, uint16_t height) {
    SDL_SetWindowSize(window, width, height);
}
