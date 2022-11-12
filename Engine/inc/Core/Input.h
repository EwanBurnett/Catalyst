#pragma once
#include <cstdint>
#include <bitset>
#include <map>
#include "Time.h"
#include "Types.h"

//TODO: Comment Interface Methods

namespace Input::Keys {
        //Define commonly used keyboard key macros
        constexpr uint8_t KB_KEY_BACK = 0x08;
        constexpr uint8_t KB_KEY_TAB = 0x09;

        constexpr uint8_t KB_KEY_ENTER = 0x0D;

        constexpr uint8_t KB_KEY_SHIFT = 0x10;
        constexpr uint8_t KB_KEY_CTRL = 0x11;

        constexpr uint8_t KB_KEY_ESC = 0x1B;

        constexpr uint8_t KB_KEY_SPACE    = 0x20;
        constexpr uint8_t KB_KEY_PGUP     = 0x21;
        constexpr uint8_t KB_KEY_PGDN     = 0x22;

        constexpr uint8_t KB_DIR_LEFT     = 0x25;
        constexpr uint8_t KB_DIR_UP       = 0x26;
        constexpr uint8_t KB_DIR_RIGHT    = 0x27;
        constexpr uint8_t KB_DIR_DOWN     = 0x28;

        constexpr uint8_t KB_KEY_PRTSCN   = 0x2C;
        constexpr uint8_t KB_KEY_INS      = 0x2D;
        constexpr uint8_t KB_KEY_DEL      = 0x2E;

        constexpr uint8_t KB_KEY_0        = 0x30;
        constexpr uint8_t KB_KEY_1        = 0x31;
        constexpr uint8_t KB_KEY_2        = 0x32;
        constexpr uint8_t KB_KEY_3        = 0x33;
        constexpr uint8_t KB_KEY_4        = 0x34;
        constexpr uint8_t KB_KEY_5        = 0x35;
        constexpr uint8_t KB_KEY_6        = 0x36;
        constexpr uint8_t KB_KEY_7        = 0x37;
        constexpr uint8_t KB_KEY_8        = 0x38;
        constexpr uint8_t KB_KEY_9        = 0x39;

        constexpr uint8_t KB_KEY_A        = 0x41;
        constexpr uint8_t KB_KEY_B        = 0x42;
        constexpr uint8_t KB_KEY_C        = 0x43;
        constexpr uint8_t KB_KEY_D        = 0x44;
        constexpr uint8_t KB_KEY_E        = 0x45;
        constexpr uint8_t KB_KEY_F        = 0x46;
        constexpr uint8_t KB_KEY_G        = 0x47;
        constexpr uint8_t KB_KEY_H        = 0x48;
        constexpr uint8_t KB_KEY_I        = 0x49;
        constexpr uint8_t KB_KEY_J        = 0x4a;
        constexpr uint8_t KB_KEY_K        = 0x4b;
        constexpr uint8_t KB_KEY_L        = 0x4c;
        constexpr uint8_t KB_KEY_M        = 0x4d;
        constexpr uint8_t KB_KEY_N        = 0x4e;
        constexpr uint8_t KB_KEY_O        = 0x4f;
        constexpr uint8_t KB_KEY_P        = 0x50;
        constexpr uint8_t KB_KEY_Q        = 0x51;
        constexpr uint8_t KB_KEY_R        = 0x52;
        constexpr uint8_t KB_KEY_S        = 0x53;
        constexpr uint8_t KB_KEY_T        = 0x54;
        constexpr uint8_t KB_KEY_U        = 0x55;
        constexpr uint8_t KB_KEY_V        = 0x56;
        constexpr uint8_t KB_KEY_W        = 0x57;
        constexpr uint8_t KB_KEY_X        = 0x58;
        constexpr uint8_t KB_KEY_Y        = 0x59;
        constexpr uint8_t KB_KEY_Z        = 0x5a;
                                                
        constexpr uint8_t KB_NUM_0        = 0x60;
        constexpr uint8_t KB_NUM_1        = 0x61;
        constexpr uint8_t KB_NUM_2        = 0x62;
        constexpr uint8_t KB_NUM_3        = 0x63;
        constexpr uint8_t KB_NUM_4        = 0x64;
        constexpr uint8_t KB_NUM_5        = 0x65;
        constexpr uint8_t KB_NUM_6        = 0x66;
        constexpr uint8_t KB_NUM_7        = 0x67;
        constexpr uint8_t KB_NUM_8        = 0x68;
        constexpr uint8_t KB_NUM_9        = 0x69;
                                                
        constexpr uint8_t KB_NUM_MUL      = 0x6A;
        constexpr uint8_t KB_NUM_ADD      = 0x6B;
        constexpr uint8_t KB_NUM_SUB      = 0x6D;
        constexpr uint8_t KB_NUM_DEC      = 0x6E;
        constexpr uint8_t KB_NUM_DIV      = 0x6F;
                                                
        constexpr uint8_t KB_FNC_F1       = 0x70;
        constexpr uint8_t KB_FNC_F2       = 0x71;
        constexpr uint8_t KB_FNC_F3       = 0x72;
        constexpr uint8_t KB_FNC_F4       = 0x73;
        constexpr uint8_t KB_FNC_F5       = 0x74;
        constexpr uint8_t KB_FNC_F6       = 0x75;
        constexpr uint8_t KB_FNC_F7       = 0x76;
        constexpr uint8_t KB_FNC_F8       = 0x77;
        constexpr uint8_t KB_FNC_F9       = 0x78;
        constexpr uint8_t KB_FNC_F10      = 0x79;
        constexpr uint8_t KB_FNC_F11      = 0x7a;
        constexpr uint8_t KB_FNC_F12      = 0x7b;
                                                
        constexpr uint8_t KB_MOD_LSHIFT   = 0xA0;
        constexpr uint8_t KB_MOD_RSHIFT   = 0xA1;
        constexpr uint8_t KB_MOD_LCONTROL = 0xA2;
        constexpr uint8_t KB_MOD_RCONTROL = 0xA3;
    //...
    }

namespace Input::MouseButtons
{
    constexpr uint8_t MOUSE_BUTTON_LEFT = 0x00;
    constexpr uint8_t MOUSE_BUTTON_RIGHT = 0x01;
    constexpr uint8_t MOUSE_BUTTON_MIDDLE = 0x02;
    constexpr uint8_t MOUSE_BUTTON_X1 = 0x03;
    constexpr uint8_t MOUSE_BUTTON_X2 = 0x04;
    constexpr uint8_t MOUSE_SCROLL = 0x05;
}

namespace Input::Keyboard
{
    bool KeyDown(const uint8_t& key);
    bool KeyPressed(const uint8_t& key);
    bool KeyReleased(const uint8_t& key);
    float KeyHeld(const uint8_t& key);
}

namespace Input::Mouse
{
    bool MouseMoved();
    float MouseButtonHeld(const uint8_t& button);
    bool MouseButtonDown(const uint8_t& button);
    bool MouseButtonPressed(const uint8_t& button);
    bool MouseButtonReleased(const uint8_t& button);
    float MouseScroll();
    Engine::Vector2i DeltaPosition();
}

namespace Input
{
    void Init(Time* timer);
    void Advance();

    //Handle the Raw input, and delegate control to whichever module needs it
    void ProcessRawInput(WPARAM wParam, LPARAM lParam);

}