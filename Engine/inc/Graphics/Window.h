#pragma once
#include "..\Framework.h"
#include "..\Core\Input.h"
#include <hidusage.h>
#include <locale>
#include <vector>
#include <string>

namespace Engine {
    //Describes the window display mode of the application.
    enum class EDisplayModes
    {
        Windowed = 0x00,
        Borderless,
        Fullscreen,
    };

    /**
     * \brief VideoMode describes the attributes of the application window:
     * \param width The width of the window (in pixels)
     * \param height The height of the window (in pixels)
     * \param refreshRate The window's refresh rate (in Hz)
     * \param adapterIndex Which display adapter to target
     * \param isFullscreen Whether the window is in fullscreen mode or not.
     */
    struct VideoMode {
        uint16_t width{ 1280 };     //Horizontal width of the window, in pixels
        uint16_t height{ 720 };     //Vertical height of the window, in pixels
        uint8_t refreshRate{ 60 };  //Window Refresh Rate (Hz)
        uint8_t adapterIndex{ 0 };  //Which adapter to target
        EDisplayModes displayMode{ EDisplayModes::Windowed };
    };

    class Window {
    public:
        friend class Graphics;
        friend class DX11_GFX;

        explicit Window(const std::basic_string<wchar_t>& title, const HINSTANCE& hInst, VideoMode mode);

        void Show(const int& showCmd) const;    
        void SetTitle(const std::basic_string<char>& title) const;
        void SetVideoMode(const VideoMode mode = {}) const;
        void SetIcon(const std::basic_string<wchar_t>& path) const;
        void ShowMouseCursor(const bool show);

    private:
        HWND m_WindowHandle = nullptr;
        Graphics* m_Gfx = nullptr;
        bool bShowCursor = true;
    };
}