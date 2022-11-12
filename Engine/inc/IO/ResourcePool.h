#pragma once
#include <d3d11_1.h>
#include <d3dx11effect.h>

//#include "..\Core\Audio.h"

#include <unordered_map>

#include <sstream>
#include <string>
#include <Vector>

#include <wrl/client.h>
#include "..\Graphics\Model.h"
#include "..\IO\Logger.h"

namespace Engine
{
    namespace ResourcePool
    {
        typedef Microsoft::WRL::ComPtr<ID3DX11Effect> shader;
        typedef Microsoft::WRL::ComPtr<ID3D11Buffer> vBuffer;
        typedef Microsoft::WRL::ComPtr<ID3D11Buffer> iBuffer;
        typedef Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
        typedef Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
        //typedef Engine::Audio::SoundPacket sound;

        void Init();
        void Shutdown();

        //TODO: Resource Deallocation
        //TODO: Comment Interface Methods

        [[nodiscard]]
         std::basic_string<wchar_t> GetShaderPath(EShaderType type);

        [[nodiscard]]
         std::basic_string<wchar_t> GetCompiledShaderPath(EShaderType type);

        [[nodiscard]]
         shader GetShader(EShaderType type);
         void AddShader(shader shader, EShaderType type);

         [[nodiscard]]
         inputLayout GetInputLayout(EShaderType type);
         void AddInputLayout(inputLayout layout, EShaderType type);

        [[nodiscard]]
         texture GetTexture(std::basic_string<wchar_t> path);
         void AddTexture(texture tex, std::basic_string<wchar_t> path);


        [[nodiscard]]
         vBuffer GetVertexBuffer(const Engine::MeshFilter* mesh);
         void AddVertexBuffer(vBuffer vertexBuffer, const Engine::MeshFilter* mesh);

        [[nodiscard]]
         iBuffer GetIndexBuffer(const Engine::MeshFilter* mesh);
         void AddIndexBuffer(iBuffer indexBuffer, const Engine::MeshFilter* mesh);

    }

}

