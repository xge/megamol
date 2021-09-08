#include "CommandRegistry.h"

#ifdef CUESDK_ENABLED
#define CORSAIR_LIGHTING_SDK_DISABLE_DEPRECATION_WARNINGS
#include "CUESDK.h"
#include <map>

// TODO move somewhere it's needed
std::map<megamol::frontend_resources::Key, CorsairLedId> CorsairLEDfromGLFWKey {
        {megamol::frontend_resources::Key::KEY_ESCAPE, CLK_Escape},
        {megamol::frontend_resources::Key::KEY_F1, CLK_F1},
        {megamol::frontend_resources::Key::KEY_F2, CLK_F2},
        {megamol::frontend_resources::Key::KEY_F3, CLK_F3},
        {megamol::frontend_resources::Key::KEY_F4, CLK_F4},
        {megamol::frontend_resources::Key::KEY_F5, CLK_F5},
        {megamol::frontend_resources::Key::KEY_F6, CLK_F6},
        {megamol::frontend_resources::Key::KEY_F7, CLK_F7},
        {megamol::frontend_resources::Key::KEY_F8, CLK_F8},
        {megamol::frontend_resources::Key::KEY_F9, CLK_F9},
        {megamol::frontend_resources::Key::KEY_F10, CLK_F10},
        {megamol::frontend_resources::Key::KEY_F11, CLK_F11},
        {megamol::frontend_resources::Key::KEY_F12, CLK_F12},
        {megamol::frontend_resources::Key::KEY_GRAVE_ACCENT, CLK_GraveAccentAndTilde},
        {megamol::frontend_resources::Key::KEY_1, CLK_1},
        {megamol::frontend_resources::Key::KEY_2, CLK_2},
        {megamol::frontend_resources::Key::KEY_3, CLK_3},
        {megamol::frontend_resources::Key::KEY_4, CLK_4},
        {megamol::frontend_resources::Key::KEY_5, CLK_5},
        {megamol::frontend_resources::Key::KEY_6, CLK_6},
        {megamol::frontend_resources::Key::KEY_7, CLK_7},
        {megamol::frontend_resources::Key::KEY_8, CLK_8},
        {megamol::frontend_resources::Key::KEY_9, CLK_9},
        {megamol::frontend_resources::Key::KEY_0, CLK_0},
        {megamol::frontend_resources::Key::KEY_MINUS, CLK_MinusAndUnderscore},
        {megamol::frontend_resources::Key::KEY_TAB, CLK_Tab},
        {megamol::frontend_resources::Key::KEY_Q, CLK_Q},
        {megamol::frontend_resources::Key::KEY_W, CLK_W},
        {megamol::frontend_resources::Key::KEY_E, CLK_E},
        {megamol::frontend_resources::Key::KEY_R, CLK_R},
        {megamol::frontend_resources::Key::KEY_T, CLK_T},
        {megamol::frontend_resources::Key::KEY_Y, CLK_Y},
        {megamol::frontend_resources::Key::KEY_U, CLK_U},
        {megamol::frontend_resources::Key::KEY_I, CLK_I},
        {megamol::frontend_resources::Key::KEY_O, CLK_O},
        {megamol::frontend_resources::Key::KEY_P, CLK_P},
        {megamol::frontend_resources::Key::KEY_LEFT_BRACKET, CLK_BracketLeft},
        {megamol::frontend_resources::Key::KEY_CAPS_LOCK, CLK_CapsLock},
        {megamol::frontend_resources::Key::KEY_A, CLK_A},
        {megamol::frontend_resources::Key::KEY_S, CLK_S},
        {megamol::frontend_resources::Key::KEY_D, CLK_D},
        {megamol::frontend_resources::Key::KEY_F, CLK_F},
        {megamol::frontend_resources::Key::KEY_G, CLK_G},
        {megamol::frontend_resources::Key::KEY_H, CLK_H},
        {megamol::frontend_resources::Key::KEY_J, CLK_J},
        {megamol::frontend_resources::Key::KEY_K, CLK_K},
        {megamol::frontend_resources::Key::KEY_L, CLK_L},
        {megamol::frontend_resources::Key::KEY_SEMICOLON, CLK_SemicolonAndColon},
        {megamol::frontend_resources::Key::KEY_APOSTROPHE, CLK_ApostropheAndDoubleQuote},
        {megamol::frontend_resources::Key::KEY_LEFT_SHIFT, CLK_LeftShift},
        {megamol::frontend_resources::Key::KEY_BACKSLASH, CLK_NonUsBackslash},
        {megamol::frontend_resources::Key::KEY_Z, CLK_Z},
        {megamol::frontend_resources::Key::KEY_X, CLK_X},
        {megamol::frontend_resources::Key::KEY_C, CLK_C},
        {megamol::frontend_resources::Key::KEY_V, CLK_V},
        {megamol::frontend_resources::Key::KEY_B, CLK_B},
        {megamol::frontend_resources::Key::KEY_N, CLK_N},
        {megamol::frontend_resources::Key::KEY_M, CLK_M},
        {megamol::frontend_resources::Key::KEY_COMMA, CLK_CommaAndLessThan},
        {megamol::frontend_resources::Key::KEY_PERIOD, CLK_PeriodAndBiggerThan},
        {megamol::frontend_resources::Key::KEY_SLASH, CLK_SlashAndQuestionMark},
        {megamol::frontend_resources::Key::KEY_LEFT_CONTROL, CLK_LeftCtrl},
        {megamol::frontend_resources::Key::KEY_LEFT_SUPER, CLK_LeftGui},
        {megamol::frontend_resources::Key::KEY_LEFT_ALT, CLK_LeftAlt},
        //{megamol::frontend_resources::Key::KEY_, CLK_Lang2},
        {megamol::frontend_resources::Key::KEY_SPACE, CLK_Space},
        //{megamol::frontend_resources::Key::KEY_, CLK_Lang1},
        //{megamol::frontend_resources::Key::KEY_, CLK_International2},
        {megamol::frontend_resources::Key::KEY_RIGHT_ALT, CLK_RightAlt},
        {megamol::frontend_resources::Key::KEY_RIGHT_SUPER, CLK_RightGui},
        //{megamol::frontend_resources::Key::KEY_, CLK_Application},
        //{megamol::frontend_resources::Key::KEY_, CLK_LedProgramming},
        //{megamol::frontend_resources::Key::KEY_, CLK_Brightness},
        //{megamol::frontend_resources::Key::KEY_, CLK_F12},
        {megamol::frontend_resources::Key::KEY_PRINT_SCREEN, CLK_PrintScreen},
        {megamol::frontend_resources::Key::KEY_SCROLL_LOCK, CLK_ScrollLock},
        {megamol::frontend_resources::Key::KEY_PAUSE, CLK_PauseBreak},
        {megamol::frontend_resources::Key::KEY_INSERT, CLK_Insert},
        {megamol::frontend_resources::Key::KEY_HOME, CLK_Home},
        {megamol::frontend_resources::Key::KEY_PAGE_UP, CLK_PageUp},
        {megamol::frontend_resources::Key::KEY_RIGHT_BRACKET, CLK_BracketRight},
        {megamol::frontend_resources::Key::KEY_BACKSLASH, CLK_Backslash},
        //{megamol::frontend_resources::Key::KEY_, CLK_NonUsTilde},
        {megamol::frontend_resources::Key::KEY_ENTER, CLK_Enter},
        //{megamol::frontend_resources::Key::KEY_, CLK_International1},
        {megamol::frontend_resources::Key::KEY_EQUAL, CLK_EqualsAndPlus},
        //{megamol::frontend_resources::Key::KEY_, CLK_International3},
        {megamol::frontend_resources::Key::KEY_BACKSPACE, CLK_Backspace},
        {megamol::frontend_resources::Key::KEY_DELETE, CLK_Delete},
        {megamol::frontend_resources::Key::KEY_END, CLK_End},
        {megamol::frontend_resources::Key::KEY_PAGE_DOWN, CLK_PageDown},
        {megamol::frontend_resources::Key::KEY_RIGHT_SHIFT, CLK_RightShift},
        {megamol::frontend_resources::Key::KEY_RIGHT_CONTROL, CLK_RightCtrl},
        {megamol::frontend_resources::Key::KEY_UP, CLK_UpArrow},
        {megamol::frontend_resources::Key::KEY_LEFT, CLK_LeftArrow},
        {megamol::frontend_resources::Key::KEY_DOWN, CLK_DownArrow},
        {megamol::frontend_resources::Key::KEY_RIGHT, CLK_RightArrow},
        //{megamol::frontend_resources::Key::KEY_, CLK_WinLock},
        //{megamol::frontend_resources::Key::KEY_, CLK_Mute},
        //{megamol::frontend_resources::Key::KEY_, CLK_Stop},
        //{megamol::frontend_resources::Key::KEY_, CLK_ScanPreviousTrack},
        //{megamol::frontend_resources::Key::KEY_, CLK_PlayPause},
        //{megamol::frontend_resources::Key::KEY_, CLK_ScanNextTrack},
        {megamol::frontend_resources::Key::KEY_NUM_LOCK, CLK_NumLock},
        {megamol::frontend_resources::Key::KEY_KP_DIVIDE, CLK_KeypadSlash},
        {megamol::frontend_resources::Key::KEY_KP_MULTIPLY, CLK_KeypadAsterisk},
        {megamol::frontend_resources::Key::KEY_KP_SUBTRACT, CLK_KeypadMinus},
        {megamol::frontend_resources::Key::KEY_KP_ADD, CLK_KeypadPlus},
        {megamol::frontend_resources::Key::KEY_KP_ENTER, CLK_KeypadEnter},
        {megamol::frontend_resources::Key::KEY_KP_7, CLK_Keypad7},
        {megamol::frontend_resources::Key::KEY_KP_8, CLK_Keypad8},
        {megamol::frontend_resources::Key::KEY_KP_9, CLK_Keypad9},
        //{megamol::frontend_resources::Key::KEY_KP_DECIMAL, CLK_KeypadComma},
        {megamol::frontend_resources::Key::KEY_KP_4, CLK_Keypad4},
        {megamol::frontend_resources::Key::KEY_KP_5, CLK_Keypad5},
        {megamol::frontend_resources::Key::KEY_KP_6, CLK_Keypad6},
        {megamol::frontend_resources::Key::KEY_KP_1, CLK_Keypad1},
        {megamol::frontend_resources::Key::KEY_KP_2, CLK_Keypad2},
        {megamol::frontend_resources::Key::KEY_KP_3, CLK_Keypad3},
        {megamol::frontend_resources::Key::KEY_KP_0, CLK_Keypad0},
        {megamol::frontend_resources::Key::KEY_KP_DECIMAL, CLK_KeypadPeriodAndDelete},
        //{megamol::frontend_resources::Key::KEY_, CLK_G1},
        //{megamol::frontend_resources::Key::KEY_, CLK_G2},
        //{megamol::frontend_resources::Key::KEY_, CLK_G3},
        //{megamol::frontend_resources::Key::KEY_, CLK_G4},
        //{megamol::frontend_resources::Key::KEY_, CLK_G5},
        //{megamol::frontend_resources::Key::KEY_, CLK_G6},
        //{megamol::frontend_resources::Key::KEY_, CLK_G7},
        //{megamol::frontend_resources::Key::KEY_, CLK_G8},
        //{megamol::frontend_resources::Key::KEY_, CLK_G9},
        //{megamol::frontend_resources::Key::KEY_, CLK_G10},
        //{megamol::frontend_resources::Key::KEY_, CLK_VolumeUp},
        //{megamol::frontend_resources::Key::KEY_, CLK_VolumeDown},
        //{megamol::frontend_resources::Key::KEY_, CLK_MR},
        //{megamol::frontend_resources::Key::KEY_, CLK_M1},
        //{megamol::frontend_resources::Key::KEY_, CLK_M2},
        //{megamol::frontend_resources::Key::KEY_, CLK_M3},
        //{megamol::frontend_resources::Key::KEY_, CLK_G11},
        //{megamol::frontend_resources::Key::KEY_, CLK_G12},
        //{megamol::frontend_resources::Key::KEY_, CLK_G13},
        //{megamol::frontend_resources::Key::KEY_, CLK_G14},
        //{megamol::frontend_resources::Key::KEY_, CLK_G15},
        //{megamol::frontend_resources::Key::KEY_, CLK_G16},
        //{megamol::frontend_resources::Key::KEY_, CLK_G17},
        //{megamol::frontend_resources::Key::KEY_, CLK_G18},
        //{megamol::frontend_resources::Key::KEY_, CLK_International5},
        //{megamol::frontend_resources::Key::KEY_, CLK_International4},
    };

#endif


megamol::frontend_resources::CommandRegistry::CommandRegistry() {
#ifdef CUESDK_ENABLED
    CorsairPerformProtocolHandshake();
    CorsairRequestControl(CAM_ExclusiveLightingControl);
#endif
}

megamol::frontend_resources::CommandRegistry::~CommandRegistry() {
#ifdef CUESDK_ENABLED
    CorsairReleaseControl(CAM_ExclusiveLightingControl);
#endif
}

void megamol::frontend_resources::CommandRegistry::add_command(const megamol::frontend_resources::Command& c) {
    const bool command_is_new = command_index.find(c.name) == command_index.end();
    const bool key_code_unused = key_to_command.find(c.key) == key_to_command.end();
    if (command_is_new && key_code_unused) {
        push_command(c);
    } else {
        Command c2;
        if (!command_is_new) {
            c2.name = increment_name(c.name);
        }
        if (key_code_unused) {
            c2.key = c.key;
        }
        c2.param = c.param;
        push_command(c2);
    }
}

void megamol::frontend_resources::CommandRegistry::remove_command(const megamol::core::param::AbstractParam* param) {
    auto it = std::find_if(commands.begin(), commands.end(), [param] (const Command& c){return c.param == param;});
    if (it != commands.end()) {
        command_index.erase(it->name);
        if (it->key.key != Key::KEY_UNKNOWN) key_to_command.erase(it->key);
        commands.erase(it);
    }
}

void megamol::frontend_resources::CommandRegistry::update_hotkey(const std::string& command_name, KeyCode key) {
    if (!is_new(command_name)) {
        auto& c = commands[command_index[command_name]];
        const auto old_key = c.key;
        c.key = key;
        key_to_command.erase(old_key);
        key_to_command[key] = command_index[command_name];
    }
}

megamol::core::param::AbstractParam* megamol::frontend_resources::CommandRegistry::param_from_keycode(KeyCode key) {
    const auto c = key_to_command.find(key);
    if (c == key_to_command.end()) {
        return nullptr;
    } else {
        return commands[c->second].param;
    }
}

std::string megamol::frontend_resources::CommandRegistry::increment_name(const std::string& oldname) {
    std::string new_name;
    std::string prefix;
    std::regex r("^(.*?)_\\d+$");
    std::cmatch m;
    if (std::regex_match(oldname.c_str(), m, r)) {
        // already have a suffix
        prefix = m[0];
    } else {
        prefix = oldname;
    }
    int cnt = 0;
    bool isnew = false;
    do {
        cnt++;
        new_name = prefix + "_" + std::to_string(cnt);
        isnew = is_new(new_name);
    } while (!isnew);
    return new_name;
}

void megamol::frontend_resources::CommandRegistry::push_command(const Command& c) {
    commands.push_back(c);
    if (c.key.key != Key::KEY_UNKNOWN) key_to_command[c.key] = static_cast<int>(commands.size() - 1);
    command_index[c.name] = static_cast<int>(commands.size() - 1);
}
