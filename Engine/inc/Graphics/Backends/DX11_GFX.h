#pragma once
#include "..\..\Framework.h"
#include "..\Graphics.h"
#include "..\..\Core\Math.h"
#include "..\..\IO\ResourcePool.h"
#include "..\..\IO\Logger.h"
#include "..\..\IO\TypeConversion.h"

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include <wrl/client.h>

#include <bitset>

namespace Engine
{
 class DX11_GFX : public Graphics
 {
 public:
     friend class Engine::Window; 
     friend class Camera;

     bool Init(Engine::Window& window, Engine::GraphicsMode mode = {}) override;
     void Draw(Matrix4x4& worldMatrix, const MeshFilter& mesh, const MeshRenderer& renderer, Camera& camera) override;
     void Draw(Matrix4x4& worldMatrix, const Sprite& sprite, Camera& camera) override;  //TODO: Use a Dynamic Buffer / Sprite Batching to eliminate draw call overheads
     void Clear(uint8_t r = 0x00, uint8_t g = 0x00, uint8_t b = 0x00, uint8_t a = 0x00) override;
     void Present() override;
     void SetGraphicsMode(const Window& window, GraphicsMode mode) override;

 private:
     
     Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
     Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext;
     Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwapChain;
     Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
     Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;

     std::vector <Microsoft::WRL::ComPtr<ID3D11DeviceContext>> m_DeferredContexts;

     D3D_FEATURE_LEVEL m_FeatureLevel = {};
     UINT m_MSAAQuality = {0};
     D3D11_TEXTURE2D_DESC m_BackBufferDesc = {};
     D3D11_VIEWPORT m_Viewport = {};
 };

 
}
