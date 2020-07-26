#pragma once

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#if defined(None)  // X11
    #undef None
#endif
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    #define SDL_VIDEO_DRIVER_X11
#elif BX_PLATFORM_WINDOWS
    #define SDL_VIDEO_DRIVER_WINDOWS
#elif BX_PLATFORM_OSX
    #define SDL_VIDEO_DRIVER_COCOA
#elif BX_PLATFORM_STEAMLINK
    #define SDL_VIDEO_DRIVER_VIVANTE
#endif
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_syswm.h>
#include <memory>
#include <InputManager.h>

namespace JuicyEngine {
class WindowManager {
    friend class Engine;
    SDL_Window* window;
    uint16_t width = 500, height = 500;
    uint32_t x = SDL_WINDOWPOS_UNDEFINED, y = x;
    uint32_t window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

    WindowManager();

public:
    //static std::shared_ptr<WindowManager> instance();

    inline uint16_t get_width() const { return width; }
    inline uint16_t get_height() const { return height; }
    
    void process(WindowEvent event);

    void resize(uint16_t width, uint16_t height);
    ~WindowManager();
};
}  // namespace JuicyEngine
