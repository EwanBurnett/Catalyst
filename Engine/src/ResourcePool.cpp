#include "../inc/IO/ResourcePool.h"

using namespace Engine;
using namespace ResourcePool;

static std::unordered_map<EShaderType, std::basic_string<wchar_t>> ShaderPaths = {
    {EShaderType::Basic, L"Resources\\Shaders\\FX\\Basic"},
    {EShaderType::Blinn, L"Resources\\Shaders\\FX\\Blinn"},
    {EShaderType::SpriteRenderer, L"Resources\\Shaders\\FX\\Sprite"}
};
static std::unordered_map<EShaderType, shader> Shaders;
static std::unordered_map<EShaderType, inputLayout> InputLayouts;
static std::unordered_map<std::basic_string<wchar_t>, texture> Textures;
static std::unordered_map<uintptr_t, vBuffer> VertexBuffers;
static std::unordered_map<uintptr_t, iBuffer> IndexBuffers;

static uint32_t nextVBufferID;
static uint32_t nextIBufferID;

void ResourcePool::Init()
{
}

void ResourcePool::Shutdown()
{
}

std::basic_string<wchar_t> ResourcePool::GetShaderPath(EShaderType type)
{
    return ShaderPaths.at(type) + L".fx";
}


 std::basic_string<wchar_t> ResourcePool::GetCompiledShaderPath(EShaderType type)
{
     return ShaderPaths.at(type) + L".cso";
}


 shader ResourcePool::GetShader(EShaderType type)
{
    if(!Shaders.contains(type))
    {
        return (shader)0;
    }
     return Shaders.at(type);
}

 void ResourcePool::AddShader(shader shader, EShaderType type)
{
     Shaders.emplace(type, shader);
}

 inputLayout ResourcePool::GetInputLayout(EShaderType type)
 {
     if (!InputLayouts.contains(type))
     {
         return (inputLayout)0;
     }
     return InputLayouts.at(type);
 }

 void ResourcePool::AddInputLayout(inputLayout layout, EShaderType type)
 {
     InputLayouts.emplace(type, layout);
 }


 texture ResourcePool::GetTexture(std::basic_string<wchar_t> path)
{
     if (!Textures.contains(path))
     {
         return (texture)0;
     }
     return Textures.at(path);
}


 void ResourcePool::AddTexture(texture tex, std::basic_string<wchar_t> path)
{
     Textures.emplace(path, tex);
}



 vBuffer ResourcePool::GetVertexBuffer(const Engine::MeshFilter* mesh)
{
     ERR(mesh == nullptr, "Mesh is invalid.");
     if (!VertexBuffers.contains((uintptr_t)mesh))
     {
         return (vBuffer)0;
     }
     return VertexBuffers.at((uintptr_t)mesh);
}

 void ResourcePool::AddVertexBuffer(vBuffer vertexBuffer, const Engine::MeshFilter* mesh)
{
     ERR(mesh == nullptr, "Mesh is invalid.");
     VertexBuffers.emplace((uintptr_t)mesh, vertexBuffer);
}



 iBuffer ResourcePool::GetIndexBuffer(const Engine::MeshFilter* mesh)
{
     ERR(mesh == nullptr, "Mesh is invalid.");
     if (!IndexBuffers.contains((uintptr_t)mesh))
     {
         return (iBuffer)0;
     }
     return IndexBuffers.at((uintptr_t)mesh);
}


 void ResourcePool::AddIndexBuffer(iBuffer indexBuffer, const Engine::MeshFilter* mesh)
{
     ERR(mesh == nullptr, "Mesh is invalid.");
    IndexBuffers.emplace((uintptr_t)mesh, indexBuffer);
}


