#pragma once
#include "..\Framework.h"
#include "Window.h"
#include "Model.h"
#include "Camera.h"
#include "../Sprite.h"

namespace Engine
{
    struct GraphicsMode
    {
        uint16_t xResolution{ 1280 };
        uint16_t yResolution{ 720 };
        uint8_t frameRate{ 60 };
        uint8_t MSAACount{ 4 };
        uint8_t MSAAQuality{ 1 };
        bool enableDepthStencil{ true };
        bool isFullscreen{ false };
    };
    class Graphics
    {
    public:
        friend class Window;

        virtual bool Init(Window& window, GraphicsMode mode = {}) = 0;
        virtual void Draw(Matrix4x4& worldMatrix, const MeshFilter& mesh, const MeshRenderer& renderer, Camera& camera) = 0;
        virtual void Draw(Matrix4x4& worldMatrix, const Sprite& sprite, Camera& camera) = 0;
        virtual void Clear(uint8_t r = 0x00, uint8_t g = 0x00, uint8_t b = 0x00, uint8_t a = 0x00) = 0;
        virtual void Present() = 0;

        virtual void SetGraphicsMode(const Window& window, GraphicsMode mode = {}) = 0;

    protected:
        GraphicsMode m_gMode;
    };
}