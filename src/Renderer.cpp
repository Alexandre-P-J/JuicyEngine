#include <Renderer.h>
#include <bx/math.h>
#include <spdlog/spdlog.h>

std::shared_ptr<Renderer> Renderer::get_instance() {
    static std::shared_ptr<Renderer> ptr(new Renderer);
    return ptr;
}



Renderer::Renderer() {
    window = SDL_CreateWindow("JuicyPixels", SDL_WINDOWPOS_UNDEFINED,
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
#endif // BX_PLATFORM_

    pd.context = NULL;
    pd.backBuffer = NULL;
    pd.backBufferDS = NULL;

    bgfx::setPlatformData(pd);
    bgfx::Init init;
    init.type = bgfx::RendererType::Count; // Automatically choose a renderer.
    init.resolution.width = w_width;
    init.resolution.height = w_height;
    init.resolution.reset = BGFX_RESET_VSYNC;
    bgfx::renderFrame(); // Render empty frame
    bgfx::init(init);

    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f,
                       0);
    bgfx::setViewRect(0, 0, 0, w_width, w_height);
}



Renderer::~Renderer() {
    bgfx::shutdown();
    destroy_window();
}



void* Renderer::init_window_handle() {
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
    wl_egl_window* win_impl =
        (wl_egl_window*)SDL_GetWindowData(_window, "wl_egl_window");
    if (!win_impl) {
        int width, height;
        SDL_GetWindowSize(_window, &width, &height);
        struct wl_surface* surface = wmi.info.wl.surface;
        if (!surface)
            return nullptr;
        win_impl = wl_egl_window_create(surface, width, height);
        SDL_SetWindowData(_window, "wl_egl_window", win_impl);
    }
    return (void*)(uintptr_t)win_impl;
#else
    return (void*)wmi.info.x11.window;
#endif
#elif BX_PLATFORM_OSX
    return wmi.info.cocoa.window;
#elif BX_PLATFORM_WINDOWS
    return wmi.info.win.window;
#endif // BX_PLATFORM_
}



void Renderer::destroy_window() {
    if (!window) {
        spdlog::critical("SDL2 window destroy attempt failed, window is null.");
    }
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#if USE_WAYLAND
    wl_egl_window* win_impl =
        (wl_egl_window*)SDL_GetWindowData(_window, "wl_egl_window");
    if (win_impl) {
        SDL_SetWindowData(_window, "wl_egl_window", nullptr);
        wl_egl_window_destroy(win_impl);
    }
#endif
#endif
    SDL_DestroyWindow(window);
    window = nullptr;
}



void Renderer::update() {
    if (active_camera) {
        auto view = active_camera->get_view(min_z_index, max_z_index);
        auto proj = active_camera->get_projection(max_z_index);
        bgfx::setViewTransform(0, &view[0], &proj[0]);
    }
    bgfx::frame();
    max_z_index = 1;
    min_z_index = -1;
}



void Renderer::resize_window(int width, int height) {
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



void Renderer::set_active_camera(std::shared_ptr<Camera> camera) {
    active_camera = camera;
}



void Renderer::set_current_buffers(bgfx::VertexBufferHandle v,
                                   bgfx::IndexBufferHandle i) {
    bgfx::setVertexBuffer(0, v);
    bgfx::setIndexBuffer(i);
}



void Renderer::set_current_transform(glm::mat4 transform, int z_index) {
    transform[3][2] = z_index;
    bgfx::setTransform(&transform[0][0]);
    max_z_index = std::max(max_z_index, z_index);
    min_z_index = std::min(min_z_index, z_index);
}



void Renderer::submit_current(bgfx::ProgramHandle shader) {
    bgfx::setState(BGFX_STATE_DEFAULT);
    bgfx::submit(0, shader);
}