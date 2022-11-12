#pragma once
#include <cstdint>
#include <immintrin.h>

namespace Engine {

    struct Vector2i
    {
        int x, y;
    };
    struct Vector2f
    {
        float x, y;
    };
    struct Vector3f
    {
        float x, y, z;

        Vector3f operator *(const float& rhs) const
        {
            return { x * rhs, y * rhs, z * rhs };
        }
        Vector3f operator +(const Vector3f& rhs) const
        {
            return { x + rhs.x, y + rhs.y, z + rhs.z };
        }
        Vector3f operator -(const Vector3f& rhs) const
        {
            return { x - rhs.x, y - rhs.y, z - rhs.z };
        }
        Vector3f operator *(const Vector3f& rhs) const 
        {
            return { x * rhs.x, y * rhs.y, z * rhs.z };
        }
    };
    struct Vector3i
    {
        int x, y, z;
    };
    struct Vector4f
    {
        float x, y, z, w;

        Vector4f operator +(const Vector4f& rhs) const
        {
            return { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
        }
        Vector4f operator *(const Vector4f& rhs) const
        {
            return { x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w };
        }
    };
    struct Vector4i
    {
        int x, y, z, w;
    };

    /**
     * \brief 32-Bit Colour - Stored in RGBA format.
     */
    struct Colour
    {
        uint8_t r, g, b, a;
    };

    struct Rect
    {
        float Left;
        float Top;
        float Right;
        float Bottom;

    };

    struct Ray
    {
        Vector3f Position, Endpoint;
    };

    struct Sphere
    {
        Vector3f Position;
        float Radius;
    };

    struct Box
    {
        Vector3f Position, Extents;

        [[nodiscard]]
        float Left() const
        {
            return Position.x;
        }

        [[nodiscard]]
        float Right() const
        {
            return Position.x + Extents.x;
        }

        [[nodiscard]]
        float Top() const
        {
            return Position.y;
        }

        [[nodiscard]]
        float Bottom() const
        {
            return Position.y + Extents.y;
        }

        [[nodiscard]]
        float Front() const
        {
            return Position.z;
        }

        [[nodiscard]]
        float Back() const
        {
            return Position.z + Extents.z;
        }


    };

    struct Frustrum
    {
        float FoVDegrees{ 90 };
        float AspectRatio{16.0f / 9.0f};
        float NearPlane{ 0.1f };
        float FarPlane{ 50000.0f };
    };
    struct Matrix4x4
    {
        Matrix4x4()
        {
            _matrix = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
            _matrix._11 = 1.0f;
            _matrix._22 = 1.0f;
            _matrix._33 = 1.0f;
            _matrix._44 = 1.0f;
        }
        union
        {
            struct
            {
                float _11, _12, _13, _14;
                float _21, _22, _23, _24;
                float _31, _32, _33, _34;
                float _41, _42, _43, _44;
            }_matrix;

            float matrix[4][4];
            
        };
    };

    
}