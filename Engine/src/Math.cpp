#include "../inc/Core/Math.h"


float Engine::Math::Clamp(float& value, float min, float max)
{
    if(value < min)
    {
        value = min;
    }
    else if(value > max)
    {
        value = max;
    }

    return value;
}



float Engine::Math::VectorLength(const Vector3f& vector)
{
    return(sqrtf((vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z)));
}

float Engine::Math::Dot(const Vector4f& a, const Vector4f& b)
{
    return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w));
}

float Engine::Math::Dot(const Vector3f& a, const Vector3f& b)
{
    return ((a.x * b.x) + (a.y * b.y) + (a.z * b.z));
}

float Engine::Math::DegToRad(const float& degrees)
{
    return(degrees * PI / 180.0f);
}

float Engine::Math::RadToDeg(const float& radians)
{
    return(radians * 180.0f / PI);
}

Engine::Vector4f Engine::Math::Normalize(const Colour& colour)
{
    Vector4f out = {};
    out.x = (float)colour.r / 255.0f;
    out.y = (float)colour.g / 255.0f;
    out.z = (float)colour.b / 255.0f;
    out.w = (float)colour.a / 255.0f;

    return out;
}

Engine::Vector3f Engine::Math::Normalize(const Vector3f& vector)
{
    Vector3f out = {};
    float len = VectorLength(vector);
    if (len > 0) {
        out.x = vector.x / len;
        out.y = vector.y / len;
        out.z = vector.z / len;
    }

    return out;
}

Engine::Vector3f Engine::Math::Cross(const Vector3f& a, const Vector3f& b)
{
    return{
        ((a.y * b.z) - (b.y * a.z)),
        -((a.x * b.z) - (b.x * a.z)),
        ((a.x * b.y) - (b.x * a.y))
    };
}

void Engine::Math::Orthonormalize(Vector3f& right, Vector3f& up, Vector3f& forward)
{
    //Ensure the forward vector is unit length
    forward = Normalize(forward);

    //Compute the correct up vector
    up = Normalize(Cross(forward, right));

    //Compute the correct right vector
    right = Cross(up, forward);
}

Engine::Vector3f Engine::Math::TransformByMatrix(const Vector3f& vector, const Matrix4x4 matrix)
{
    Vector4f out = {};

    Vector4f x = { vector.x, vector.x, vector.x, vector.x };
    Vector4f y = { vector.y, vector.y, vector.y, vector.y };
    Vector4f z = { vector.z, vector.z, vector.z, vector.z };

    Vector4f row1 = { matrix._matrix._11, matrix._matrix._12, matrix._matrix._13, matrix._matrix._14 };
    Vector4f row2 = { matrix._matrix._21, matrix._matrix._22, matrix._matrix._23, matrix._matrix._24 };
    Vector4f row3 = { matrix._matrix._31, matrix._matrix._32, matrix._matrix._33, matrix._matrix._34 };

    out = z * row3;
    out = (y * row2) + out;
    out = (x * row1) + out;

    return { out.x, out.y, out.z };
}

Engine::Vector3f Engine::Math::MatrixMultiply(const Vector3f& vector, const Matrix4x4& matrix)
{
    Vector4f v = { vector.x, vector.y, vector.z, 0.0f };
    auto out = MatrixMultiply(v, matrix);
    return {out.x, out.y, out.z};
}

Engine::Vector4f Engine::Math::MatrixMultiply(const Vector4f& vector, const Matrix4x4& matrix)
{
    Vector4f out;

    out.x = Dot(
        vector,
        { matrix._matrix._11,matrix._matrix._21,matrix._matrix._31,matrix._matrix._41 });
    out.y = Dot(
        vector,
        { matrix._matrix._12,matrix._matrix._22,matrix._matrix._32,matrix._matrix._42 });
    out.z = Dot(
        vector,
        { matrix._matrix._13,matrix._matrix._23,matrix._matrix._33,matrix._matrix._43 });
    out.w = Dot(
        vector,
        { matrix._matrix._14,matrix._matrix._24,matrix._matrix._34,matrix._matrix._44 });


    return out;
    
}


Engine::Matrix4x4 Engine::Math::MatrixMultiply(const Matrix4x4& a, const Matrix4x4& b)
{
    Matrix4x4 out;
    //TODO: MMult Using SIMD Intrinsics

    //Row 1
    out._matrix._11 = Dot(
        { a._matrix._11,a._matrix._12,a._matrix._13,a._matrix._14},
        { b._matrix._11,b._matrix._21,b._matrix._31,b._matrix._41 });

    out._matrix._12 = Dot(
        { a._matrix._11,a._matrix._12,a._matrix._13,a._matrix._14 },
        { b._matrix._12,b._matrix._22,b._matrix._32,b._matrix._42 });

    out._matrix._13 = Dot(
        { a._matrix._11,a._matrix._12,a._matrix._13,a._matrix._14 },
        { b._matrix._13,b._matrix._23,b._matrix._33,b._matrix._43 });

    out._matrix._14 = Dot(
        { a._matrix._11,a._matrix._12,a._matrix._13,a._matrix._14 },
        { b._matrix._14,b._matrix._24,b._matrix._34,b._matrix._44 });

    //Row 2
    out._matrix._21 = Dot(
        { a._matrix._21,a._matrix._22,a._matrix._23,a._matrix._24 },
        { b._matrix._11,b._matrix._21,b._matrix._31,b._matrix._41 });

    out._matrix._22 = Dot(
        { a._matrix._21,a._matrix._22,a._matrix._23,a._matrix._24 },
        { b._matrix._12,b._matrix._22,b._matrix._32,b._matrix._42 });

    out._matrix._23 = Dot(
        { a._matrix._21,a._matrix._22,a._matrix._23,a._matrix._24 },
        { b._matrix._13,b._matrix._23,b._matrix._33,b._matrix._43 });

    out._matrix._24 = Dot(
        { a._matrix._21,a._matrix._22,a._matrix._23,a._matrix._24 },
        { b._matrix._14,b._matrix._24,b._matrix._34,b._matrix._44 });

    //Row 3
    out._matrix._31 = Dot(
        { a._matrix._31,a._matrix._32,a._matrix._33,a._matrix._34 },
        { b._matrix._11,b._matrix._21,b._matrix._31,b._matrix._41 });

    out._matrix._32 = Dot(
        { a._matrix._31,a._matrix._32,a._matrix._33,a._matrix._34 },
        { b._matrix._12,b._matrix._22,b._matrix._32,b._matrix._42 });

    out._matrix._33 = Dot(
        { a._matrix._31,a._matrix._32,a._matrix._33,a._matrix._34 },
        { b._matrix._13,b._matrix._23,b._matrix._33,b._matrix._43 });

    out._matrix._34 = Dot(
        { a._matrix._31,a._matrix._32,a._matrix._33,a._matrix._34 },
        { b._matrix._14,b._matrix._24,b._matrix._34,b._matrix._44 });

    //Row 4
    out._matrix._41 = Dot(
        { a._matrix._41,a._matrix._42,a._matrix._43,a._matrix._44 },
        { b._matrix._11,b._matrix._21,b._matrix._31,b._matrix._41 });

    out._matrix._42 = Dot(
        { a._matrix._41,a._matrix._42,a._matrix._43,a._matrix._44 },
        { b._matrix._12,b._matrix._22,b._matrix._32,b._matrix._42 });

    out._matrix._43 = Dot(
        { a._matrix._41,a._matrix._42,a._matrix._43,a._matrix._44 },
        { b._matrix._13,b._matrix._23,b._matrix._33,b._matrix._43 });

    out._matrix._44 = Dot(
        { a._matrix._41,a._matrix._42,a._matrix._43,a._matrix._44 },
        { b._matrix._14,b._matrix._24,b._matrix._34,b._matrix._44 });


    return out;
}

Engine::Matrix4x4 Engine::Math::MatrixTranslation(const Vector3f& translation)
{
    Matrix4x4 out = {};

    out._matrix._41 = translation.x;
    out._matrix._42 = translation.y;
    out._matrix._43 = translation.z;

    return out;
}

Engine::Matrix4x4 Engine::Math::MatrixRotation(const Vector3f& rotation)
{
    //X Rotation
    Matrix4x4 xRot = {};
    {
        xRot._matrix._22 = cosf(DegToRad(rotation.x));
        xRot._matrix._23 = sinf(DegToRad(rotation.x));
        xRot._matrix._32 = -sinf(DegToRad(rotation.x));
        xRot._matrix._33 = cosf(DegToRad(rotation.x));
    }

    //Y Rotation
    Matrix4x4 yRot = {};
    {
        yRot._matrix._11 = cosf(DegToRad(rotation.y));
        yRot._matrix._13 = -sinf(DegToRad(rotation.y));
        yRot._matrix._31 = sinf(DegToRad(rotation.y));
        yRot._matrix._33 = cosf(DegToRad(rotation.y));
    }

    //Z Rotation
    Matrix4x4 zRot = {};
    {
        zRot._matrix._11 = cosf(DegToRad(rotation.z));
        zRot._matrix._12 = sinf(DegToRad(rotation.z));
        zRot._matrix._21 = -sinf(DegToRad(rotation.z));
        zRot._matrix._22 = cosf(DegToRad(rotation.z));
    }

    return MatrixMultiply(xRot, MatrixMultiply(yRot, zRot));

}

Engine::Matrix4x4 Engine::Math::MatrixScaling(const Vector3f& scaling)
{
    Matrix4x4 out = {};

    out._matrix._11 = scaling.x;
    out._matrix._22 = scaling.y;
    out._matrix._33 = scaling.z;

    return out;
}

/**
 * \brief Constructs a View Matrix from the input vectors.
 * \param Origin The Position of the viewer
 * \param Right The Right Vector 
 * \param Up The Up Vector
 * \param Forward The Forward Vector
 * \return A View Matrix from the perspective of the viewer.
 */
Engine::Matrix4x4 Engine::Math::MatrixView(const Vector3f& origin, Vector3f& right, Vector3f& up, Vector3f& forward)
{
    //Re-Orthonormalize the input vectors
    Orthonormalize(right, up, forward);

    //Compute the position
    Vector3f position = {};
    position.x = Dot(origin, right);
    position.y = Dot(origin, up);
    position.z = Dot(origin, forward);

    //Construct the matrix
    Matrix4x4 out = {};

    out._matrix._11 = right.x;
    out._matrix._21 = right.y;
    out._matrix._31 = right.z;
    out._matrix._41 = -position.x;

    out._matrix._12 = up.x;
    out._matrix._22 = up.y;
    out._matrix._32 = up.z;
    out._matrix._42 = -position.y;

    out._matrix._13 = forward.x;
    out._matrix._23 = forward.y;
    out._matrix._33 = forward.z;
    out._matrix._43 = -position.z;

    out._matrix._14 = 0.0f;
    out._matrix._24 = 0.0f;
    out._matrix._34 = 0.0f;
    out._matrix._44 = 1.0f;

    return out;
}

Engine::Matrix4x4 Engine::Math::MatrixProjection(const Frustrum& frustrum)
{
    Matrix4x4 out = {};

    float a = tanf(DegToRad(frustrum.FoVDegrees) / 2.0f);

    out._matrix._11 = 1.0f / frustrum.AspectRatio * a;
    out._matrix._12 = 0.0f;
    out._matrix._13 = 0.0f;
    out._matrix._14 = 0.0f;

    out._matrix._21 = 0.0f;
    out._matrix._22 = 1.0f / a;
    out._matrix._23 = 0.0f;
    out._matrix._24 = 0.0f;

    out._matrix._31 = 0.0f;
    out._matrix._32 = 0.0f;
    out._matrix._33 = frustrum.FarPlane / (frustrum.FarPlane - frustrum.NearPlane);
    out._matrix._34 = 1.0f;

    out._matrix._41 = 0.0f;
    out._matrix._42 = 0.0f;
    out._matrix._43 = -frustrum.NearPlane* frustrum.FarPlane / (frustrum.FarPlane - frustrum.NearPlane);
    out._matrix._44 = 0.0f;

    return out;
}

Engine::Matrix4x4 Engine::Math::MatrixProjectionOrthographic(const Frustrum& frustrum, const Rect& viewPort)
{
    Matrix4x4 out = {};

    //Construct the matrix
    out._matrix._11 = 2.0f / (viewPort.Right - viewPort.Left);
    out._matrix._12 = 0.0f;
    out._matrix._13 = 0.0f;
    out._matrix._14 = 0.0f;

    out._matrix._21 = 0.0f;
    out._matrix._22 = 2.0f / (viewPort.Top - viewPort.Bottom);
    out._matrix._23 = 0.0f;
    out._matrix._24 = 0.0f;

    out._matrix._31 = 0.0f;
    out._matrix._32 = 0.0f;
    out._matrix._33 = 1.0f;
    out._matrix._34 = 0.0f;

    out._matrix._41 = -((viewPort.Right + viewPort.Left) / (viewPort.Right - viewPort.Left));
    out._matrix._42 = -((viewPort.Top + viewPort.Bottom) / (viewPort.Top - viewPort.Bottom));
    out._matrix._43 = 0.0f;
    out._matrix._44 = 1.0f;


    //Construct the matrix
    //out._matrix._11 = vpWidth + vpHeight;
    //out._matrix._12 = 0.0f;
    //out._matrix._13 = 0.0f;
    //out._matrix._14 = 0.0f;

    //out._matrix._21 = 0.0f;
    //out._matrix._22 = vpWidth + vpHeight;
    //out._matrix._23 = 0.0f;
    //out._matrix._24 = 0.0f;

    //out._matrix._31 = 0.0f;
    //out._matrix._32 = 0.0f;
    //out._matrix._33 = 1.0f / (frustrum.FarPlane - frustrum.NearPlane);
    //out._matrix._34 = 0.0f;

    //out._matrix._41 = -(viewPort.Left + viewPort.Right) * vpWidth;
    //out._matrix._42 = -(viewPort.Top + viewPort.Bottom) * vpHeight;
    //out._matrix._43 = -out._matrix._33 * frustrum.NearPlane;
    //out._matrix._44 = 1.0f;

    return out;
}
