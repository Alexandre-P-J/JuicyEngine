#include "InputManager.h"


#include <SDL.h>
#include "SDL_keyboard.h"
#include <Engine.h>
#include <WindowManager.h>
#include <spdlog/spdlog.h>

JuicyEngine::InputManager::InputManager() {
    if (!SDL_WasInit(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER |
                     SDL_INIT_EVENTS)) {
        spdlog::critical(
            "SDL_JOYSTICK/GAMECONTROLLER/EVENTS not initialized before "
            "InputManager construction");
    }
}

void JuicyEngine::InputManager::refresh_input() {
    input = std::queue<Event>();
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_WINDOWEVENT: {
                auto we =
                    static_cast<JuicyEngine::WindowEvent>(event.window.event);
                input.push(we);
                auto wm = JuicyEngine::Engine::instance().get_window_manager();
                wm->process(we);
                continue;
            }
            case SDL_KEYUP:
            case SDL_KEYDOWN: {
                JuicyEngine::KeyboardEvent ke = {
                    event.type == SDL_KEYDOWN, event.key.repeat,
                    static_cast<JuicyEngine::KeyboardKey>(event.key.keysym.sym),
                    static_cast<JuicyEngine::KeyboardModKey>(
                        event.key.keysym.mod)};
                input.push(ke);
                continue;
            }
            case SDL_MOUSEMOTION: {
                auto const& s = event.motion.state;
                JuicyEngine::MouseMotionEvent mme{bool(s & SDL_BUTTON_LMASK),
                                                  bool(s & SDL_BUTTON_MMASK),
                                                  bool(s & SDL_BUTTON_RMASK),
                                                  bool(s & SDL_BUTTON_X1MASK),
                                                  bool(s & SDL_BUTTON_X2MASK),
                                                  event.motion.x,
                                                  event.motion.y,
                                                  event.motion.xrel,
                                                  event.motion.yrel};
                input.push(mme);
                continue;
            }
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN: {
                JuicyEngine::MouseButtonEvent mbe{
                    event.type == SDL_MOUSEBUTTONDOWN,
                    static_cast<JuicyEngine::MouseButton>(event.button.button),
                    event.button.clicks, event.button.x, event.button.y};
                input.push(mbe);
                continue;
            }
            case SDL_MOUSEWHEEL: {
                JuicyEngine::MouseWheelEvent mwe{event.wheel.x, event.wheel.y};
                input.push(mwe);
                continue;
            }
            case SDL_JOYAXISMOTION: {
                JuicyEngine::JoyAxisEvent jae{event.jaxis.axis,
                                              event.jaxis.value / 32768.f};
                input.push(jae);
                continue;
            }
            case SDL_JOYBALLMOTION: {
                JuicyEngine::JoyBallEvent jbe{
                    event.jball.ball, event.jball.xrel, event.jball.yrel};
                input.push(jbe);
                continue;
            }
            case SDL_JOYHATMOTION: {
                JuicyEngine::JoyHatEvent jhe{
                    event.jhat.hat,
                    static_cast<JoyHatPosition>(event.jhat.value)};
                input.push(jhe);
                continue;
            }
            case SDL_JOYBUTTONUP:
            case SDL_JOYBUTTONDOWN: {
                JuicyEngine::JoyButtonEvent jbe{
                    event.jbutton.button, event.type == SDL_JOYBUTTONDOWN};
                input.push(jbe);
                continue;
            }
            case SDL_JOYDEVICEREMOVED:
            case SDL_JOYDEVICEADDED: {
                JuicyEngine::JoyDeviceEvent jde{
                    event.type == SDL_JOYDEVICEADDED, event.jdevice.which};
                input.push(jde);
                continue;
            }
            case SDL_CONTROLLERAXISMOTION: {
                JuicyEngine::ControllerAxisEvent cae{
                    event.caxis.axis, event.caxis.value / 32768.f};
                input.push(cae);
                continue;
            }
            case SDL_CONTROLLERBUTTONUP:
            case SDL_CONTROLLERBUTTONDOWN: {
                JuicyEngine::ControllerButtonEvent cbe{
                    event.type == SDL_CONTROLLERBUTTONDOWN,
                    static_cast<ControllerButton>(event.cbutton.button)};
                input.push(cbe);
                continue;
            }
            case SDL_CONTROLLERDEVICEADDED:
            case SDL_CONTROLLERDEVICEREMAPPED:
            case SDL_CONTROLLERDEVICEREMOVED: {
                JuicyEngine::ControllerDeviceEvent cde{
                    static_cast<ControllerDeviceState>(event.type),
                    event.cdevice.which};
                input.push(cde);
                continue;
            }
            case SDL_QUIT: {
                JuicyEngine::Engine::instance().exit();
                continue;
            }
            default:
                // NOT IMPLEMENTED:
                // SDL_TextEditingEvent (UI)
                // SDL_TextInputEvent (UI)
                // SDL_AudioDeviceEvent (sdl audio)
                // SDL_TouchFingerEvent (tablet/mobile)
                // SDL_MultiGestureEvent (tablet/mobile)
                // SDL_DollarGestureEvent (tablet/mobile)
                // SDL_DropEvent (drag-drop file into game window)
                continue;
        }
    }
}

std::queue<JuicyEngine::Event>& JuicyEngine::InputManager::get_events() {
    return input;
}
