#include "../inc/Core/Input.h"
#include "../inc/Core/Time.h"

constexpr uint16_t MAX_KEYS = 256;
constexpr uint8_t MAX_BUTTONS = 0x05;

//TODO: Comment Implementation Methods

namespace Input::Keyboard
{
    std::bitset<MAX_KEYS> m_Inputs[3];
    std::map<uint8_t, float> m_InputQueue;
    Time* m_Time;

    void Advance();
}

namespace Input::Mouse
{
    void Advance();

    Time* m_Time;

    Engine::Vector2i lastPos = { 0, 0 };
    Engine::Vector2i thisPos = { 0, 0 };
    std::map<uint8_t, float> m_InputQueue;
    std::bitset<6> m_MouseKeys[3];
    float m_MouseScroll;

}

//KEYBOARD INPUT -----------------------------------------------------

void ProcessKey(USHORT status, UINT code)
{
    if (status == RI_KEY_MAKE)
    {
        printf("Key %c : DOWN \n", code);
        Input::Keyboard::m_Inputs[0].set(code, true);
    }
    else if (status == RI_KEY_BREAK)
    {
        printf("Key %c : UP \n", code);
        Input::Keyboard::m_Inputs[0].set(code, false);
    }

}


void Input::Keyboard::Advance()
{
    //'Advance' inputs, moving the members of the buffer up.
    m_Inputs[2] = m_Inputs[1];
    m_Inputs[1] = m_Inputs[0];
}

bool Input::Keyboard::KeyPressed(const uint8_t& key) {

    //If the key is out of range, then just return false.
    if (key > MAX_KEYS)
    {
        return false;
    }

    //If the key wasn't pressed during the last frame, but has been this frame, return true
    if (m_Inputs[1].test(key) == true && m_Inputs[2].test(key) == false)
    {
        return true;
    }

    return false;
}

bool Input::Keyboard::KeyReleased(const uint8_t& key)
{
    //If the key is out of range, then just return false.
    if (key > MAX_KEYS)
    {
        return false;
    }

    //If the key wasn't pressed during the last frame, but has been this frame, return true
    if (m_Inputs[1].test(key) == false && m_Inputs[2].test(key) == true)
    {
        return true;
    }

    return false;
}

float Input::Keyboard::KeyHeld(const uint8_t& key)
{
    //If the key is out of range, then just return false.
    if (key > MAX_KEYS)
    {
        return 0.0f;
    }

    //Test to see whether the key is being held (was pressed this frame, on the frame before, and the frame before)
    if (m_Inputs[1].test(key) == true && m_Inputs[2].test(key) == true)
    {
        //If it's already being held in the input map, Add its held time
        if (m_InputQueue.contains(key))
        {
            m_InputQueue[key] += m_Time->DeltaTime();
        }
        //Else, add it to the input map
        else
        {
            m_InputQueue.emplace(key, 0.0f);
        }
    }

    if (KeyDown(key) == false)
    {
        if (m_InputQueue.contains(key))
        {
            m_InputQueue[key] = 0.0f;
        }
    }

    return m_InputQueue[key];
}

bool Input::Keyboard::KeyDown(const uint8_t& key)
{
    //If the key is out of range, then just return false.
    if (key > MAX_KEYS)
    {
        return false;
    }

    return(m_Inputs[1].test(key));
}


//MOUSE INPUT --------------------------------------------------------

void ProcessMouse(int xPos, int yPos, USHORT buttonFlags, USHORT buttonData)
{
    Input::Mouse::thisPos.x = xPos;
    Input::Mouse::thisPos.y = yPos;

    //Set mouse key state appropriately
    if (buttonFlags & RI_MOUSE_BUTTON_1_DOWN)
    {
        Input::Mouse::m_MouseKeys[0].set(Input::MouseButtons::MOUSE_BUTTON_LEFT, true);
    }
    if (buttonFlags & RI_MOUSE_BUTTON_1_UP)
    {
        Input::Mouse::m_MouseKeys[0].set(Input::MouseButtons::MOUSE_BUTTON_LEFT, false);
    }
    if (buttonFlags & RI_MOUSE_BUTTON_2_DOWN)
    {
        Input::Mouse::m_MouseKeys[0].set(Input::MouseButtons::MOUSE_BUTTON_RIGHT, true);
    }
    if (buttonFlags & RI_MOUSE_BUTTON_2_UP)
    {
        Input::Mouse::m_MouseKeys[0].set(Input::MouseButtons::MOUSE_BUTTON_RIGHT, false);
    }
    if (buttonFlags & RI_MOUSE_BUTTON_3_DOWN)
    {
        Input::Mouse::m_MouseKeys[0].set(Input::MouseButtons::MOUSE_BUTTON_MIDDLE, true);
    }
    if (buttonFlags & RI_MOUSE_BUTTON_3_UP)
    {
        Input::Mouse::m_MouseKeys[0].set(Input::MouseButtons::MOUSE_BUTTON_MIDDLE, false);
    }
    if (buttonFlags & RI_MOUSE_BUTTON_4_DOWN)
    {
        Input::Mouse::m_MouseKeys[0].set(Input::MouseButtons::MOUSE_BUTTON_X1, true);
    }
    if (buttonFlags & RI_MOUSE_BUTTON_4_UP)
    {
        Input::Mouse::m_MouseKeys[0].set(Input::MouseButtons::MOUSE_BUTTON_X1, false);
    }
    if (buttonFlags & RI_MOUSE_BUTTON_5_DOWN)
    {
        Input::Mouse::m_MouseKeys[0].set(Input::MouseButtons::MOUSE_BUTTON_X2, true);
    }
    if (buttonFlags & RI_MOUSE_BUTTON_5_UP)
    {
        Input::Mouse::m_MouseKeys[0].set(Input::MouseButtons::MOUSE_BUTTON_X2, false);
    }
    if (buttonFlags & RI_MOUSE_WHEEL)
    {
        Input::Mouse::m_MouseScroll += (float)(short)buttonData;
    }
}

bool Input::Mouse::MouseButtonDown(const uint8_t& button)
{
    //If the input query exceeds our range, return false.
    if(button > MAX_BUTTONS)
    {
        return false;   
    }

    //Return the state of the button.
    return(m_MouseKeys[1][button]);
}

bool Input::Mouse::MouseButtonPressed(const uint8_t& button)
{
    //If the input query exceeds our range, return false.
    if (button > MAX_BUTTONS)
    {
        return false;
    }

    //Return the state of the button.
    return((m_MouseKeys[1].test(button) == true) && (m_MouseKeys[2].test(button) == false));
}

bool Input::Mouse::MouseButtonReleased(const uint8_t& button)
{
    //If the input query exceeds our range, return false.
    if (button > MAX_BUTTONS)
    {
        return false;
    }

    //Return the state of the button.
    return((m_MouseKeys[1].test(button) == false) && (m_MouseKeys[2].test(button) == true));
}

float Input::Mouse::MouseScroll()
{
    return m_MouseScroll;
}


void Input::Mouse::Advance()
{
    lastPos = thisPos;
    thisPos = { 0, 0 };

    m_MouseKeys[2] = m_MouseKeys[1];
    m_MouseKeys[1] = m_MouseKeys[0];

    m_MouseScroll = 0;
}

bool Input::Mouse::MouseMoved()
{
    if (lastPos.x == 0 && thisPos.x == 0 && lastPos.y == 0 && thisPos.y == 0)
    {
        return false;
    }
    return true;
}

float Input::Mouse::MouseButtonHeld(const uint8_t& button)
{
    //If the key is out of range, then just return false.
    if (button > MAX_BUTTONS)
    {
        return false;
    }

    //Test to see whether the key is being held (was pressed this frame, on the frame before, and the frame before)
    if (m_MouseKeys[1].test(button) == true && m_MouseKeys[2].test(button) == true)
    {
        //If it's already being held in the input map, Add its held time
        if (m_InputQueue.contains(button))
        {
            m_InputQueue[button] += m_Time->DeltaTime();
        }
        //Else, add it to the input map
        else
        {
            m_InputQueue.emplace(button, 0.0f);
        }
        
    }

    if (MouseButtonDown(button) == false)
    {
        if (m_InputQueue.contains(button))
        {
            m_InputQueue[button] = 0.0f;
        }
    }

    return m_InputQueue[button];
}

Engine::Vector2i Input::Mouse::DeltaPosition()
{
    Engine::Vector2i pos;
    pos.x = thisPos.x - lastPos.x;
    pos.y = thisPos.y - lastPos.y;
    return pos;
}




//INTERFACE -----------------------------------------------------------

void Input::Init(Time* timer)
{
    //Propagate the timer
    Input::Keyboard::m_Time = timer;
    Input::Mouse::m_Time = timer;

    //Clear the input cache
    Input::Keyboard::m_Inputs[0].reset();
    Input::Mouse::m_MouseKeys[0].reset();
}

void Input::Advance()
{
    Keyboard::Advance();
    printf("\n---------------\n%s\n", Keyboard::m_Inputs[2].to_string().c_str());
    Mouse::Advance();
}

void Input::ProcessRawInput(WPARAM wParam, LPARAM lParam)

{
    UINT inputSize = {0};
    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &inputSize, sizeof(RAWINPUTHEADER));
    LPBYTE lpInput = new BYTE[inputSize];
    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpInput, &inputSize, sizeof(RAWINPUTHEADER));
    RAWINPUT* pInput = (RAWINPUT*)lpInput;

    switch (pInput->header.dwType)
    {
    case RIM_TYPEKEYBOARD:
        ProcessKey(pInput->data.keyboard.Flags, pInput->data.keyboard.VKey);
        break;
    case RIM_TYPEMOUSE:
        ProcessMouse(pInput->data.mouse.lLastX, pInput->data.mouse.lLastY, pInput->data.mouse.usButtonFlags, pInput->data.mouse.usButtonData);
        break;
    case RIM_TYPEHID:
        break;
    default:
        break;
    }

    delete[](lpInput);
}
