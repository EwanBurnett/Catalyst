#include "../inc/graphics/Window.h"
#include "../inc/Graphics/Backends/DX11_GFX.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


/**
 * \brief Window Procedure used in handling messages for the application. Forwards input and window resizing methods to the appropriate interfaces.
 */
LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_DESTROY:
        PostQuitMessage(0xdead);
        return 0;
    case WM_KEYDOWN:
        //Input::ProcessKey(true, wParam);
        break;
    case WM_KEYUP:
        //Input::ProcessKey(false, wParam);
        break;
    case WM_SIZE:   //TODO: When the user resizes the window

        break;
    case WM_SETFOCUS:   //TODO: Configurable Cursor Settings
        ShowCursor(false);    //Hide the cursor when this window is focused.
        break;
    case WM_KILLFOCUS:
        ClipCursor(NULL);
        ShowCursor(true);   //Display the cursor when focus is lost.
    case WM_INPUT:
        if (GetFocus() == hWnd) {   //Only process input when focused
            //Constrain the cursor to the window
            RECT wndRect;
            GetWindowRect(hWnd, &wndRect);  //TODO: Clip cursor to center of window
            ClipCursor(&wndRect);
            Input::ProcessRawInput(wParam, lParam);
        }

        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

/**
 * \brief Queries the system for monitor data, placing it into a structure.
 * \param pData a std::vector of RECTs, each describing a connected monitor
 * \return
 */
static BOOL CALLBACK EnumerateMonitors(HMONITOR hMonitor, HDC hdc, LPRECT lpRect, LPARAM pData)
{
    //Retrieve the monitor vector pointer from pData
    ((std::vector<RECT>*)pData)->emplace_back(*lpRect);
    return TRUE;
}

/**
 * \brief Computes the coordinates required to center a window
 * \param mode the VideoMode of the window to center
 * \return a POINT structure containing the centered coordinates.
 */
POINT GetWindowCenter(const Engine::VideoMode mode = {}) {
    //Retrieve the size of all connected displays, in pixels.
    std::vector<RECT> monitors;
    int idx = mode.adapterIndex;
    EnumDisplayMonitors(NULL, NULL, EnumerateMonitors, reinterpret_cast<LPARAM>(&monitors));

    //Get the display adapter specified by the adapterIndex in the VideoMode
    if (mode.adapterIndex > monitors.size() - 1)
    {
        //Fallback to the default display
        idx = 0;
    }
    const RECT display = monitors.at(idx);

    //Center the window on the enumerated display
    return { (((display.right - display.left) - mode.width) / 2) + display.left, ((display.bottom - mode.height) / 2) };
}

//Struct containing Window position data
struct WindowPositionInfo
{
    UINT x, y, width, height;
    DWORD style;
};

/**
 * \brief Repositions the window based on video mode
 * \param mode Input video mode
 * \return A WindowPositionInfo Struct containing the new window position
 */
WindowPositionInfo PositionWindow(const Engine::VideoMode mode = {})
{
    WindowPositionInfo out = {};

    //TODO: support multiple monitors in borderless: Using SM_CXSCREEN / SM_CYSCREEN only returns the size of the primary display. If display sizes are mismatched, the program will render incorrectly while using borderless mode.
    UINT width = mode.displayMode == Engine::EDisplayModes::Windowed ? mode.width : GetSystemMetrics(SM_CXSCREEN);
    UINT height = mode.displayMode == Engine::EDisplayModes::Windowed ? mode.height : GetSystemMetrics(SM_CYSCREEN);

    //Evaluate the window style
    DWORD windowStyle = 0x00;
    mode.displayMode == Engine::EDisplayModes::Windowed ? windowStyle |= WS_CAPTION | WS_THICKFRAME | WS_SYSMENU : NULL;
    mode.displayMode == Engine::EDisplayModes::Borderless ? windowStyle |= WS_POPUP | WS_BORDER : NULL;
    //Create a normal window for fullscreen mode - which is handled by the graphics API
    mode.displayMode == Engine::EDisplayModes::Fullscreen ? windowStyle |= WS_CAPTION | WS_THICKFRAME | WS_SYSMENU : NULL;

    //Compute the window rect
    RECT windowRect = { 0, 0, (LONG)width, (LONG)height };
    AdjustWindowRect(&windowRect, windowStyle, FALSE);

    Engine::VideoMode vm = mode;
    vm.width = (uint16_t)width;
    vm.height = (uint16_t)height;

    const POINT center = GetWindowCenter(vm);

    out.x = center.x;
    out.y = center.y;
    out.width = windowRect.right - windowRect.left;
    out.height = windowRect.bottom - windowRect.top;
    out.style = windowStyle;

    return out;
}

/**
 * \brief Creates a win32 window.
 * \param title The title of the window
 * \param hInst HINST supplied by WinMain()
 * \param mode The VideoMode of the window
 * \return the window handle to the created window.
 */
HWND InitWindow(const std::basic_string<wchar_t> title, const HINSTANCE& hInst, const Engine::VideoMode mode = {})
{
    std::basic_string<wchar_t> windowClassName = L"Engine Window";  //TODO: populate from configuration file

    //Register a new window class
    WNDCLASSEX wnd = {};
    {
        ZeroMemory(&wnd, sizeof(wnd));
        wnd.cbSize = sizeof(WNDCLASSEX);
        wnd.style = CS_OWNDC;
        wnd.lpfnWndProc = WndProc;
        wnd.hInstance = hInst;
        wnd.hIcon = nullptr;
        wnd.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wnd.lpszClassName = windowClassName.c_str();
    }

    RegisterClassEx(&wnd);

    auto windowPosition = PositionWindow(mode);

    //Create the window 
    const auto windowHandle = CreateWindow(windowClassName.c_str(), title.c_str(), windowPosition.style, windowPosition.x, windowPosition.y, windowPosition.width, windowPosition.height, nullptr, nullptr, hInst, nullptr);


    //Register raw input devices - 0 = mouse, 1 = keyboard
    RAWINPUTDEVICE rid[2];
    rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
    rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
    rid[0].dwFlags = RIDEV_INPUTSINK;
    rid[0].hwndTarget = windowHandle;

    rid[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
    rid[1].usUsage = HID_USAGE_GENERIC_KEYBOARD;
    rid[1].dwFlags = RIDEV_INPUTSINK;
    rid[1].hwndTarget = windowHandle;

    RegisterRawInputDevices(rid, 2, sizeof(rid[0]));
    //ERR((RegisterRawInputDevices(rid, 2, sizeof(rid[0])) == false), "Raw Input Device Registration Failed!");

    return windowHandle;
}



//WINDOW CLASS METHODS --------------------------------------------------------------------------------

/**
 * \brief Creates a new window
 * \param title The Title of the Window.
 * \param hInst HINSTANCE supplied by WinMain
 * \param mode The VideoMode of the Window
 */
Engine::Window::Window(const std::basic_string<wchar_t>& title, const HINSTANCE& hInst, VideoMode mode)
{
    //Create the window, and cache its window handle
    m_WindowHandle = InitWindow(title, hInst);

    SetVideoMode(mode);
}

/**
 * \brief Displays the window to the screen. Must be called after Window::Create()
 * \param showCmd Window show flags - supplied by WinMain by default.
 */
void Engine::Window::Show(const int& showCmd) const {
    ERR(m_WindowHandle == nullptr, "Window Handle is invalid.\n Did you call Window::Create()?\n");
    if (m_WindowHandle != nullptr) {
        ShowWindow(m_WindowHandle, showCmd);
    }
}

/**
 * \brief Changes the title of the window.
 * \param title The new title of the window. Cannot be empty.
 */
void Engine::Window::SetTitle(const std::basic_string<char>& title) const {
    //Change the window's title
    ERR(m_WindowHandle == nullptr, "Window Handle is invalid.\n Did you call Window::Create()?\n");
    if (!title.empty()) {
        SetWindowTextA(m_WindowHandle, title.c_str());
    }
}

/**
 * \brief Changes the VideoMode of the window - effectively resizing it.
 * \param mode The VideoMode to change to
 */
void Engine::Window::SetVideoMode(const VideoMode mode) const {
    ERR(m_WindowHandle == nullptr, "Window Handle is invalid.\n Did you call Window::Create()?\n");

    //Change the graphics mode appropriately
    if (m_Gfx != nullptr) {
        GraphicsMode gMode = m_Gfx->m_gMode;
        if (mode.displayMode == EDisplayModes::Fullscreen && m_Gfx->m_gMode.isFullscreen == false) {
            gMode.isFullscreen = true;

            m_Gfx->SetGraphicsMode(*this, gMode);
        }
        else if (mode.displayMode != EDisplayModes::Fullscreen && m_Gfx->m_gMode.isFullscreen == true)
        {
            gMode.isFullscreen = false;

            m_Gfx->SetGraphicsMode(*this, gMode);
        }

    }

    //Resize the window
    auto pos = PositionWindow(mode);

    SetWindowLong(m_WindowHandle, GWL_STYLE, pos.style);
    SetWindowPos(m_WindowHandle, nullptr, pos.x, pos.y, pos.width, pos.height, SWP_FRAMECHANGED | SWP_SHOWWINDOW);


}

/**
 * \brief Sets the window's icon
 * \param path Path to a .ico file to load.
 */
void Engine::Window::SetIcon(const std::basic_string<wchar_t>& path) const
{
    //Load the icon from the supplied filepath
    HICON icoSmall = reinterpret_cast<HICON>(LoadImage(nullptr, path.c_str(), IMAGE_ICON, 16, 16, LR_LOADFROMFILE));
    HICON icoLarge = reinterpret_cast<HICON>(LoadImage(nullptr, path.c_str(), IMAGE_ICON, 32, 32, LR_LOADFROMFILE));

    //Set the window's icon
    SendMessage(m_WindowHandle, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icoSmall));
    SendMessage(m_WindowHandle, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icoLarge));
}

/**
* \brief Shows or hides the mouse cursor.
* \param show the state of the mouse cursor
*/
void Engine::Window::ShowMouseCursor(const bool show) {
    ShowCursor(show);
}

