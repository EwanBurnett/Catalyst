#pragma once
#include "../Core/Math.h"
#include "../Core/Types.h"

namespace Engine {

    struct Transform
    {
        Vector3f Position = { 0, 0, 0 };
        Vector3f EulerRotation = { 0, 0, 0 };
        Vector3f Scale = { 1.0f, 1.0f, 1.0f };

        Matrix4x4 World = {};

        void ComputeWorld()
        {
            const Matrix4x4 translation = Math::MatrixTranslation(Position);
            const Matrix4x4 eulerRotation = Math::MatrixRotation(EulerRotation);
            const Matrix4x4 scaling = Math::MatrixScaling(Scale);

            //W = SRT
            World = Math::MatrixMultiply(scaling, Math::MatrixMultiply(eulerRotation, translation));
        }
    };

}