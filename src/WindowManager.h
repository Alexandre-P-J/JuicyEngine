#pragma once

#include <bgfx/platform.h>
#include <bgfx/bgfx.h>
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

namespace Engine {
class WindowManager {
    SDL_Window* window;
    uint16_t width, height = 500;
    uint32_t x, y = SDL_WINDOWPOS_UNDEFINED;
    uint32_t window_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

    WindowManager();
    void resize(uint16_t width, uint16_t height);
    ~WindowManager();
};
}  // namespace Engine
