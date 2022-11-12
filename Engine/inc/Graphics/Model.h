#pragma once
#include "..\Core\Types.h"
#include "..\Core\Math.h"
#include <string>
#include <vector>

namespace Engine {

    enum class EVertexAttributes
    {
        Position = 0,
        TexCoord,
        Normal,
        Tangent,
        Binormal,
    };
    enum class EPrimitiveTopology
    {
        Points = 0,
        TriangleStrip,
        TriangleList,
        LineStrip,
        LineList,
    };
    enum class EShaderType
    {
        Basic = 0,
        Blinn,
        SpriteRenderer,
    };


    struct MeshFilter
    {
        std::basic_string<char> Name;   

        std::vector<Vector3f> Vertices;
        std::vector<Vector3f> Normals;
        std::vector<Vector3f> Tangents;
        std::vector<Vector3f> Binormals;
        std::vector<Vector2f> TexCoords;
        std::vector<Vector4f> VertexColours;
        std::vector<uint32_t> Indices;

        uint32_t FaceCount = 0;
        uint32_t MaterialIndex = 0;
    };

    struct MaterialData {};

    struct MeshRenderer
    {
        ~MeshRenderer()
        {
            //delete(material); //TODO: Clean up
        }
        EPrimitiveTopology topology = EPrimitiveTopology::TriangleList;
        EShaderType shader = EShaderType::Basic;
        MaterialData* material = nullptr;
        std::basic_string<char> technique;  //Quality level
    };

    struct Basic : public Engine::MaterialData
    {
        Engine::Colour Diffuse = { 0xff, 0xff, 0xff, 0xff };
    };

    struct Blinn : public Engine::MaterialData
    {
        Engine::Colour Ambient = { 0xff, 0xff, 0xff, 0xff };
        Engine::Colour Diffuse = { 0xff, 0xff, 0xff, 0xff };
        Engine::Colour Specular = { 0xff, 0xff, 0xff, 0xff };
        float SpecularPower = 255.0f;
        std::basic_string<wchar_t> DiffuseMap;
        std::basic_string<wchar_t> NormalMap;
        std::basic_string<wchar_t> SpecularMap;
    };

    struct SpriteRenderer : public Engine::MaterialData
    {
        Engine::Colour Diffuse = { 0xff, 0xff, 0xff, 0xff };
        std::basic_string<wchar_t> TextureAtlas;
    };

    class Model
    {
    public:
        std::vector<MeshFilter> meshes;
        std::vector<MeshRenderer> renderers;
        Matrix4x4 worldMatrix;
        std::basic_string<char> source;
        void ComputeWorld(const Vector3f& position = {}, const Vector3f& rotation = {}, const Vector3f& scale = {1.0f, 1.0f, 1.0f})
        {
            const Matrix4x4 translation = Math::MatrixTranslation(position);
            const Matrix4x4 eulerRotation = Math::MatrixRotation(rotation);
            const Matrix4x4 scaling = Math::MatrixScaling(scale);

            worldMatrix = Math::MatrixMultiply(translation, Math::MatrixMultiply(eulerRotation, scaling));
        }
    };


    namespace Primitives
    {
        struct Quad : public MeshFilter
        {
            Quad()
            {
                Name = "PRIMITIVE_QUAD";

                Vertices = {
                    {0.0f, 1.0f, 0.0f},
                    {1.0f, 1.0f, 0.0f},
                    {1.0f, 0.0f, 0.0f},
                    {0.0f, 0.0f, 0.0f},
                };

                TexCoords = {
                    {0.0f, 0.0f},
                    {0.0f, 1.0f },
                    {1.0f, 1.0f },
                    {1.0f, 0.0f },
                };

                Normals = {
                    {0.0f, 0.0f, 1.0f},
                    {0.0f, 0.0f, 1.0f},
                    {0.0f, 0.0f, 1.0f},
                    {0.0f, 0.0f, 1.0f},
                };

                Tangents = {
                    {0.0f, 1.0f, 0.0f},
                    {0.0f, 1.0f, 0.0f},
                    {0.0f, 1.0f, 0.0f},
                    {0.0f, 1.0f, 0.0f},
                };

                Binormals = {
                    {1.0f, 0.0f, 0.0f},
                    {1.0f, 0.0f, 0.0f},
                    {1.0f, 0.0f, 0.0f},
                    {1.0f, 0.0f, 0.0f},
                };

                Indices = {
                    0, 1, 2,
                    0, 2, 3
                };
            }
        };

        struct Cube : public MeshFilter
        {
            
                Cube(){
                
                    Name = "PRIMITIVE_CUBE";
                    Vertices = {
                        {-0.5f, 0.5f, 0.5f},
                        {0.5f, 0.5f, 0.5f},
                        {-0.5f, -0.5f, 0.5f},
                        {0.5f, -0.5f, 0.5f},
                        {-0.5f, 0.5f, -0.5f},
                        {0.5f, 0.5f, -0.5f},
                        {-0.5f, -0.5f, -0.5f},
                        {0.5f, -0.5f, -0.5f},
                    };

                    Normals = {
                        {-0.577f, 0.577f, 0.577f },
                        {0.577f, 0.577f, 0.577f },
                        {-0.577f, -0.577f, 0.577f },
                        {0.577f, -0.577f, 0.577f },
                        {-0.577f, 0.577f, -0.577f },
                        {0.577f, 0.577f, -0.577f },
                        {-0.577f, -0.577f, -0.577f },
                        {0.577f, -0.577f, -0.577f },
                    };

                    Indices = {
                        0, 2, 3,
                        0, 3, 1,

                        0, 4, 5,
                        0, 5, 1,

                        0, 4, 6,
                        0, 6, 2,

                        2, 6, 7,
                        2, 7, 3,

                        1, 5, 7,
                        1, 7, 3,

                        4, 6, 7,
                        4, 7, 5,
                    };

                    TexCoords = {
                {0,0},
                {0,1},
                {1,1},
                {1,0},
                {0,0},
                {0,1},
                {1,1},
                {1,0},
                {0,0},
                {0,1},
                {1,1},
                {1,0},
                {0,0},
                {0,1},
                {1,1},
                {1,0},
                {0,0},
                {0,1},
                {1,1},
                {1,0},
                {0,0},
                {0,1},
                {1,1},
                {1,0}
                    };
                
                }
        };

        struct Plane : public MeshFilter
        {

            Plane(float width, float depth, uint32_t rows, uint32_t slices)
            {
                Name = "PRIMITIVE_PLANE";
                Name.append(std::to_string(rows));
                Name.append("X");
                Name.append(std::to_string(slices));

                auto vertexCount = rows * slices;
                auto faceCount = (rows - 1) * (slices - 1) * 2;
                //
                // Create the vertices.
                //
                float halfWidth = 0.5f * width;
                float halfDepth = 0.5f * depth;
                float dx = width / (slices - 1);
                float dz = depth / (rows - 1);
                float du = 1.0f / (slices - 1);
                float dv = 1.0f / (rows - 1);

                Vertices.resize(vertexCount);
                Normals.resize(vertexCount);
                Tangents.resize(vertexCount);
                TexCoords.resize(vertexCount);
                for (uint32_t i = 0; i < rows; ++i)
                {
                    float z = halfDepth - i * dz;
                    for (uint32_t j = 0; j < slices; ++j)
                    {
                        float x = -halfWidth + j * dx;
                        const auto idx = i * slices + j;

                        Vertices[idx] = { x, 0.0f, z };
                        // Ignore for now, used for lighting.
                        Normals[idx] = { 0.0f, 1.0f, 0.0f };
                        Tangents[idx] = { 1.0f, 0.0f, 0.0f };
                        // Ignore for now, used for texturing.
                        TexCoords[idx].x = j * du;
                        TexCoords[idx].y = i * dv;
                    }
                }
                
                // Iterate over each quad and compute indices.
                uint32_t k = 0;
                for (uint32_t i = 0; i < rows - 1; ++i)
                {
                    for (uint32_t j = 0; j < slices - 1; ++j)
                    {
                        Indices.emplace_back(i * slices + j);
                        Indices.emplace_back(i * slices + j + 1);
                        Indices.emplace_back((i + 1) * slices + j);
                        Indices.emplace_back((i + 1) * slices + j);
                        Indices.emplace_back(i * slices + j + 1);
                        Indices.emplace_back((i + 1) * slices + j + 1);
                        k += 6; // next quad
                    }
                }
            }
        };

    }
}