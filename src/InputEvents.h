#include <SDL.h>
#include <SDL_keyboard.h>
#include "SDL_events.h"
#include "SDL_joystick.h"

namespace JuicyEngine {

enum class WindowEvent {
    shown = SDL_WINDOWEVENT_SHOWN,
    hidden = SDL_WINDOWEVENT_HIDDEN,
    exposed = SDL_WINDOWEVENT_EXPOSED,
    moved = SDL_WINDOWEVENT_MOVED,
    resized = SDL_WINDOWEVENT_RESIZED,
    size_changed = SDL_WINDOWEVENT_SIZE_CHANGED,
    minimized = SDL_WINDOWEVENT_MINIMIZED,
    maximized = SDL_WINDOWEVENT_MAXIMIZED,
    restored = SDL_WINDOWEVENT_RESTORED,
    enter = SDL_WINDOWEVENT_ENTER,
    leave = SDL_WINDOWEVENT_LEAVE,
    focus_gained = SDL_WINDOWEVENT_FOCUS_GAINED,
    focus_lost = SDL_WINDOWEVENT_FOCUS_LOST,
    close = SDL_WINDOWEVENT_CLOSE,
    take_focus = SDL_WINDOWEVENT_TAKE_FOCUS,
    hit_test = SDL_WINDOWEVENT_HIT_TEST
};

enum class KeyboardModKey {
    none = KMOD_NONE,
    lshift = KMOD_LSHIFT,
    rshift = KMOD_RSHIFT,
    lctrl = KMOD_LCTRL,
    rctrl = KMOD_RCTRL,
    lalt = KMOD_LALT,
    ralt = KMOD_RALT,
    lgui = KMOD_LGUI,
    rgui = KMOD_RGUI,
    num = KMOD_NUM,
    caps = KMOD_CAPS,
    mode = KMOD_MODE,
    ctrl = KMOD_CTRL,
    shift = KMOD_SHIFT,
    alt = KMOD_ALT,
    gui = KMOD_GUI
};

enum class KeyboardKey {
    key_0 = SDLK_0,
    key_1 = SDLK_1,
    key_2 = SDLK_2,
    key_3 = SDLK_3,
    key_4 = SDLK_4,
    key_5 = SDLK_5,
    key_6 = SDLK_6,
    key_7 = SDLK_7,
    key_8 = SDLK_8,
    key_9 = SDLK_9,
    a = SDLK_a,
    app_control_back = SDLK_AC_BACK,
    app_control_bookmarks = SDLK_AC_BOOKMARKS,
    app_control_forward = SDLK_AC_FORWARD,
    app_control_home = SDLK_AC_HOME,
    app_control_refresh = SDLK_AC_REFRESH,
    app_control_search = SDLK_AC_SEARCH,
    app_control_stop = SDLK_AC_STOP,
    again = SDLK_AGAIN,
    alterase = SDLK_ALTERASE,
    quote = SDLK_QUOTE,
    application = SDLK_APPLICATION,
    audiomute = SDLK_AUDIOMUTE,
    audionext = SDLK_AUDIONEXT,
    audioplay = SDLK_AUDIOPLAY,
    audioprev = SDLK_AUDIOPREV,
    audiostop = SDLK_AUDIOSTOP,
    b = SDLK_b,
    backslash = SDLK_BACKSLASH,
    backspace = SDLK_BACKSPACE,
    brightnessdown = SDLK_BRIGHTNESSDOWN,
    brightnessup = SDLK_BRIGHTNESSUP,
    c = SDLK_c,
    calculator = SDLK_CALCULATOR,
    cancel = SDLK_CANCEL,
    capslock = SDLK_CAPSLOCK,
    clear = SDLK_CLEAR,
    clearagain = SDLK_CLEARAGAIN,
    comma = SDLK_COMMA,
    computer = SDLK_COMPUTER,
    copy = SDLK_COPY,
    crsel = SDLK_CRSEL,
    currencysubunit = SDLK_CURRENCYSUBUNIT,
    currencyunit = SDLK_CURRENCYUNIT,
    cut = SDLK_CUT,
    d = SDLK_d,
    decimalseparator = SDLK_DECIMALSEPARATOR,
    key_delete = SDLK_DELETE,
    displayswitch = SDLK_DISPLAYSWITCH,
    down = SDLK_DOWN,
    e = SDLK_e,
    eject = SDLK_EJECT,
    end = SDLK_END,
    equals = SDLK_EQUALS,
    escape = SDLK_ESCAPE,
    execute = SDLK_EXECUTE,
    exsel = SDLK_EXSEL,
    f = SDLK_f,
    f1 = SDLK_F1,
    f10 = SDLK_F10,
    f11 = SDLK_F11,
    f12 = SDLK_F12,
    f13 = SDLK_F13,
    f14 = SDLK_F14,
    f15 = SDLK_F15,
    f16 = SDLK_F16,
    f17 = SDLK_F17,
    f18 = SDLK_F18,
    f19 = SDLK_F19,
    f2 = SDLK_F2,
    f20 = SDLK_F20,
    f21 = SDLK_F21,
    f22 = SDLK_F22,
    f23 = SDLK_F23,
    f24 = SDLK_F24,
    f3 = SDLK_F3,
    f4 = SDLK_F4,
    f5 = SDLK_F5,
    f6 = SDLK_F6,
    f7 = SDLK_F7,
    f8 = SDLK_F8,
    f9 = SDLK_F9,
    find = SDLK_FIND,
    g = SDLK_g,
    backquote = SDLK_BACKQUOTE,
    h = SDLK_h,
    help = SDLK_HELP,
    home = SDLK_HOME,
    i = SDLK_i,
    insert = SDLK_INSERT,
    j = SDLK_j,
    k = SDLK_k,
    kbdillumdown = SDLK_KBDILLUMDOWN,
    kbdillumtoggle = SDLK_KBDILLUMTOGGLE,
    kbdillumup = SDLK_KBDILLUMUP,
    keypad_0 = SDLK_KP_0,
    keypad_00 = SDLK_KP_00,
    keypad_000 = SDLK_KP_000,
    keypad_1 = SDLK_KP_1,
    keypad_2 = SDLK_KP_2,
    keypad_3 = SDLK_KP_3,
    keypad_4 = SDLK_KP_4,
    keypad_5 = SDLK_KP_5,
    keypad_6 = SDLK_KP_6,
    keypad_7 = SDLK_KP_7,
    keypad_8 = SDLK_KP_8,
    keypad_9 = SDLK_KP_9,
    keypad_a = SDLK_KP_A,
    keypad_ampersand = SDLK_KP_AMPERSAND,
    keypad_at = SDLK_KP_AT,
    keypad_b = SDLK_KP_B,
    keypad_backspace = SDLK_KP_BACKSPACE,
    keypad_binary = SDLK_KP_BINARY,
    keypad_c = SDLK_KP_C,
    keypad_clear = SDLK_KP_CLEAR,
    keypad_clearentry = SDLK_KP_CLEARENTRY,
    keypad_colon = SDLK_KP_COLON,
    keypad_comma = SDLK_KP_COMMA,
    keypad_d = SDLK_KP_D,
    keypad_dblampersand = SDLK_KP_DBLAMPERSAND,
    keypad_dblverticalbar = SDLK_KP_DBLVERTICALBAR,
    keypad_decimal = SDLK_KP_DECIMAL,
    keypad_divide = SDLK_KP_DIVIDE,
    keypad_e = SDLK_KP_E,
    keypad_enter = SDLK_KP_ENTER,
    keypad_equals = SDLK_KP_EQUALS,
    keypad_equalsas400 = SDLK_KP_EQUALSAS400,
    keypad_exclam = SDLK_KP_EXCLAM,
    keypad_f = SDLK_KP_F,
    keypad_greater = SDLK_KP_GREATER,
    keypad_hash = SDLK_KP_HASH,
    keypad_hexadecimal = SDLK_KP_HEXADECIMAL,
    keypad_leftbrace = SDLK_KP_LEFTBRACE,
    keypad_leftparen = SDLK_KP_LEFTPAREN,
    keypad_less = SDLK_KP_LESS,
    keypad_memadd = SDLK_KP_MEMADD,
    keypad_memclear = SDLK_KP_MEMCLEAR,
    keypad_memdivide = SDLK_KP_MEMDIVIDE,
    keypad_memmultiply = SDLK_KP_MEMMULTIPLY,
    keypad_memrecall = SDLK_KP_MEMRECALL,
    keypad_memstore = SDLK_KP_MEMSTORE,
    keypad_memsubtract = SDLK_KP_MEMSUBTRACT,
    keypad_minus = SDLK_KP_MINUS,
    keypad_multiply = SDLK_KP_MULTIPLY,
    keypad_octal = SDLK_KP_OCTAL,
    keypad_percent = SDLK_KP_PERCENT,
    keypad_period = SDLK_KP_PERIOD,
    keypad_plus = SDLK_KP_PLUS,
    keypad_plusminus = SDLK_KP_PLUSMINUS,
    keypad_power = SDLK_KP_POWER,
    keypad_rightbrace = SDLK_KP_RIGHTBRACE,
    keypad_rightparen = SDLK_KP_RIGHTPAREN,
    keypad_space = SDLK_KP_SPACE,
    keypad_tab = SDLK_KP_TAB,
    keypad_verticalbar = SDLK_KP_VERTICALBAR,
    keypad_xor = SDLK_KP_XOR,
    l = SDLK_l,
    lalt = SDLK_LALT,
    lctrl = SDLK_LCTRL,
    left = SDLK_LEFT,
    leftbracket = SDLK_LEFTBRACKET,
    lgui = SDLK_LGUI,
    lshift = SDLK_LSHIFT,
    m = SDLK_m,
    mail = SDLK_MAIL,
    mediaselect = SDLK_MEDIASELECT,
    menu = SDLK_MENU,
    minus = SDLK_MINUS,
    mode = SDLK_MODE,
    mute = SDLK_MUTE,
    n = SDLK_n,
    numlockclear = SDLK_NUMLOCKCLEAR,
    o = SDLK_o,
    oper = SDLK_OPER,
    out = SDLK_OUT,
    p = SDLK_p,
    pagedown = SDLK_PAGEDOWN,
    pageup = SDLK_PAGEUP,
    paste = SDLK_PASTE,
    pause = SDLK_PAUSE,
    period = SDLK_PERIOD,
    power = SDLK_POWER,
    printscreen = SDLK_PRINTSCREEN,
    prior = SDLK_PRIOR,
    q = SDLK_q,
    r = SDLK_r,
    ralt = SDLK_RALT,
    rctrl = SDLK_RCTRL,
    key_return = SDLK_RETURN,
    return2 = SDLK_RETURN2,
    rgui = SDLK_RGUI,
    right = SDLK_RIGHT,
    rightbracket = SDLK_RIGHTBRACKET,
    rshift = SDLK_RSHIFT,
    s = SDLK_s,
    scrolllock = SDLK_SCROLLLOCK,
    select = SDLK_SELECT,
    semicolon = SDLK_SEMICOLON,
    separator = SDLK_SEPARATOR,
    slash = SDLK_SLASH,
    sleep = SDLK_SLEEP,
    space = SDLK_SPACE,
    stop = SDLK_STOP,
    sysreq = SDLK_SYSREQ,
    t = SDLK_t,
    tab = SDLK_TAB,
    thousandsseparator = SDLK_THOUSANDSSEPARATOR,
    u = SDLK_u,
    undo = SDLK_UNDO,
    unknown = SDLK_UNKNOWN,
    up = SDLK_UP,
    v = SDLK_v,
    volumedown = SDLK_VOLUMEDOWN,
    volumeup = SDLK_VOLUMEUP,
    w = SDLK_w,
    www = SDLK_WWW,
    x = SDLK_x,
    y = SDLK_y,
    z = SDLK_z,
    ampersand = SDLK_AMPERSAND,
    asterisk = SDLK_ASTERISK,
    at = SDLK_AT,
    caret = SDLK_CARET,
    colon = SDLK_COLON,
    dollar = SDLK_DOLLAR,
    exclaim = SDLK_EXCLAIM,
    greater = SDLK_GREATER,
    hash = SDLK_HASH,
    leftparen = SDLK_LEFTPAREN,
    less = SDLK_LESS,
    percent = SDLK_PERCENT,
    plus = SDLK_PLUS,
    question = SDLK_QUESTION,
    quotedbl = SDLK_QUOTEDBL,
    rightparen = SDLK_RIGHTPAREN,
    underscore = SDLK_UNDERSCORE
};

struct KeyboardEvent {
    bool pressed;
    uint8_t repeat;
    KeyboardKey key;
    KeyboardModKey modifier;
};

struct MouseMotionEvent {
    bool left_pressed, middle_pressed, right_pressed, X1_pressed, X2_pressed;
    int32_t x, y;
    int32_t dx, dy;
};

enum class MouseButton {
    left = SDL_BUTTON_LEFT,
    middle = SDL_BUTTON_MIDDLE,
    right = SDL_BUTTON_RIGHT,
    X1 = SDL_BUTTON_X1,
    X2 = SDL_BUTTON_X2
};

struct MouseButtonEvent {
    bool pressed;
    MouseButton button;
    uint8_t clicks;
    int32_t x, y;
};

struct MouseWheelEvent {
    int32_t x, y;
};

struct JoyAxisEvent {
    uint8_t axis;
    float strength;
};

struct JoyBallEvent {
    uint8_t ball;
    int16_t dx, dy;
};

enum class JoyHatPosition {
    left_up = SDL_HAT_LEFTUP,
    left = SDL_HAT_LEFT,
    left_down = SDL_HAT_DOWN,
    up = SDL_HAT_UP,
    centered = SDL_HAT_CENTERED,
    down = SDL_HAT_DOWN,
    right_up = SDL_HAT_RIGHTUP,
    right = SDL_HAT_RIGHT,
    right_down = SDL_HAT_RIGHTDOWN
};

struct JoyHatEvent {
    uint8_t hat;
    JoyHatPosition position;
};

struct JoyButtonEvent {
    uint8_t button;
    bool pressed;
};

struct JoyDeviceEvent {
    bool connected;
    int32_t id;
};

struct ControllerAxisEvent {
    uint8_t axis;
    float strength;
};

enum class ControllerButton {
    invalid = SDL_CONTROLLER_BUTTON_INVALID,
    a = SDL_CONTROLLER_BUTTON_A,
    b = SDL_CONTROLLER_BUTTON_B,
    x = SDL_CONTROLLER_BUTTON_X,
    y = SDL_CONTROLLER_BUTTON_Y,
    back = SDL_CONTROLLER_BUTTON_BACK,
    guide = SDL_CONTROLLER_BUTTON_GUIDE,
    start = SDL_CONTROLLER_BUTTON_START,
    leftstick = SDL_CONTROLLER_BUTTON_LEFTSTICK,
    rightstick = SDL_CONTROLLER_BUTTON_RIGHTSTICK,
    leftshoulder = SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
    rightshoulder = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
    dpad_up = SDL_CONTROLLER_BUTTON_DPAD_UP,
    dpad_down = SDL_CONTROLLER_BUTTON_DPAD_DOWN,
    dpad_left = SDL_CONTROLLER_BUTTON_DPAD_LEFT,
    dpad_right = SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
    max = SDL_CONTROLLER_BUTTON_MAX
};

struct ControllerButtonEvent {
    bool pressed;
    ControllerButton button;
};

enum class ControllerDeviceState {
    connected = SDL_CONTROLLERDEVICEADDED,
    disconnected = SDL_CONTROLLERDEVICEREMOVED,
    remapped = SDL_CONTROLLERDEVICEREMAPPED
};

struct ControllerDeviceEvent {
    ControllerDeviceState state;
    int32_t id;
};

}  // namespace JuicyEngine
