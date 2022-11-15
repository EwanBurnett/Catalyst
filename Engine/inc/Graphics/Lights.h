#pragma once
#include "../Core/Types.h"

#define NUM_POINT_LIGHTS 8
#define NUM_SPOT_LIGHTS 8

namespace Engine {
    struct Light {};

    struct DirectionalLight : public Light {
        Vector4f colour;
        Vector3f direction;
    };

    struct PointLight : public Light {
        Vector4f colour;
        Vector3f position;
        float radius;
    };

    struct SpotLight : public Light {
        Vector3f direction;
        float innerAngle;
        float outerAngle;
    };
    
    struct Lights {
        DirectionalLight directional;
        PointLight pointLights[NUM_POINT_LIGHTS];
        SpotLight spotLights[NUM_SPOT_LIGHTS];
    };

}