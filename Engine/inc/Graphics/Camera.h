#pragma once
#include "..\Core\Types.h"
#include <DirectXMath.h>    //TODO: remove this dependency

namespace Engine {
    class Camera
    {
    public:
        friend class DX11_GFX;

        Camera(const Engine::VideoMode videoMode = {})
        {
            mode = videoMode;
            viewFrustrum.AspectRatio = (float)videoMode.width / (float)videoMode.height;
            ComputeViewProjection();
        }
        Vector3f Position{0.0f, 0.0f, 0.0f};
        Vector3f Orientation{ 0.0f, 0.0f, 0.0f };

        Vector3f Forward{0.0f, 0.0f, 1.0f};   //Forward Vector
        Vector3f Up{0.0f, 1.0f, 0.0f};  //Up Vector
        Vector3f Right{ 1.0f, 0.0f, 0.0f }; //Right Vector
        
        VideoMode mode;
        Frustrum viewFrustrum;

    
        Matrix4x4 viewMatrix;
        Matrix4x4 projMatrix;
        Matrix4x4 orthoProjMatrix;

        void Walk(Engine::Vector3f dir = {}, float speed = 0.0f)
        {
            Math::Clamp(speed, 0.0f, 10000.0f);
            Vector3f s = { speed, speed, speed };

            dir = Math::Normalize(dir);

            if (dir.z != 0)
            {
                Position = Position + (Forward * dir.z) * s;
            }
            if (dir.y != 0)
            {
                Position = Position + (Up * dir.y) * s;
            }
            if (dir.x != 0)
            {
                Position = Position + (Right * dir.x) * s;
            }
            

        }

        void Orbit(Engine::Vector3f focus, float theta, float phi, float radius = 10.0f)
        {
            //TODO: Orbit the camera around the scene
            //Spherical Coordinates to Cartesian Coordinates
            Vector3f pos = {};
            pos.x = radius * sinf(phi) * cosf(theta);
            pos.y = radius * sinf(phi) * cosf(theta);
            pos.z = radius * cosf(phi);

            Position = pos;
            Up = { 0, 1, 0 };
            Forward = Math::Normalize(Position - focus) * -1.0f;

        }
        void Look(const int& x, const int& y, float speed = 1.0f)
        {
            Math::Clamp(speed, 0.001f, 1000.0f);

            Orientation.y += -x * speed;//(3.1415926 / mode.height) * speed * y * -1.0f;
            Orientation.x += -y * speed;//(Math::PI * 2 / mode.width) * speed * x * -1.0f;

            Math::Clamp(Orientation.x, -90.0f, 90.0f);  //Lock the camera to look between directly up, or down.

            //Compute the look direction
            auto rotation = Math::MatrixRotation(Orientation);

            const Vector3f up = { 0, 1, 0 };
            const Vector3f fwd = { 0, 0, 1 };
            const Vector3f rgt = { 1, 0, 0 };

            Up = Math::MatrixMultiply(up, rotation);
            Forward = Math::MatrixMultiply(fwd, rotation);
            Right = Math::MatrixMultiply(rgt, rotation);
        }

        void ComputeViewProjection()
        {
            viewMatrix = Math::MatrixView(Position, Right, Up, Forward);
            projMatrix = Math::MatrixProjection(viewFrustrum);
            orthoProjMatrix = Math::MatrixProjectionOrthographic(viewFrustrum, {0, 0, 1600, 900});  //TODO: Replace the viewport parameter with the screen's width and height
        }

    };
    
}