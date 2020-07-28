#pragma once
#include "InputEvents.h"
#include <queue>
#include <variant>

namespace JuicyEngine {

using Event =
    std::variant<WindowEvent, KeyboardEvent, MouseMotionEvent, MouseButtonEvent,
                 MouseWheelEvent, JoyAxisEvent, JoyBallEvent, JoyHatEvent,
                 JoyButtonEvent, JoyDeviceEvent, ControllerAxisEvent,
                 ControllerButtonEvent, ControllerDeviceEvent>;

class InputManager {
    friend class Engine;
    std::queue<Event> input;
    InputManager();

public:
    void refresh_input();
    std::queue<Event>& get_events();
};
}  // namespace JuicyEngine
