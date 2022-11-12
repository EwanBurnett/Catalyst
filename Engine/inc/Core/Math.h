#pragma once
#include "Types.h"
#include <cmath>
//TODO: Comment Interface Methods

namespace Engine::Math
{
    constexpr float PI = 3.14159265f;

    float Clamp(float& value, float min, float max);

    float VectorLength(const Vector3f& vector);
    float Dot(const Vector4f& a, const Vector4f& b);
    float Dot(const Vector3f& a, const Vector3f& b);

    float DegToRad(const float& degrees);
    float RadToDeg(const float& radians);

    Vector4f Normalize(const Colour& colour);
    Vector3f Normalize(const Vector3f& vector);

    Vector3f Cross(const Vector3f& a, const Vector3f& b);

    void Orthonormalize(Vector3f& right, Vector3f& up, Vector3f& forward);

    Vector3f TransformByMatrix(const Vector3f& vector, const Matrix4x4 matrix);

    Vector3f MatrixMultiply(const Vector3f& vector, const Matrix4x4& matrix);
    Vector4f MatrixMultiply(const Vector4f& vector, const Matrix4x4& matrix);
    Matrix4x4 MatrixMultiply(const Matrix4x4& a, const Matrix4x4& b);

    Matrix4x4 MatrixTranslation(const Vector3f& translation);
    Matrix4x4 MatrixRotation(const Vector3f& rotation);
    Matrix4x4 MatrixScaling(const Vector3f& scaling);

    Matrix4x4 MatrixView(const Vector3f& origin, Vector3f& right, Vector3f& up, Vector3f& forward);
    Matrix4x4 MatrixProjection(const Frustrum& frustrum);
    Matrix4x4 MatrixProjectionOrthographic(const Frustrum& frustrum, const Rect& viewPort);
}
