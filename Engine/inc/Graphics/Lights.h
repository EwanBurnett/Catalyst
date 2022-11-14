#pragma once
#include "../Core/Types.h"

namespace Engine {
    struct Light {
        Colour colour;
    };

    struct DirectionalLight : public Light {
        Vector3f direction
    };

    struct PointLight : public Light {
        Vector3f position;
        float radius;
    };

    struct SpotLight : public PointLight {
        Vector3f direction;
        float innerAngle;
        float outerAngle;
    };


}