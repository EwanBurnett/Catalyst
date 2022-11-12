#pragma once
#include "Types.h"
#include "Math.h"

namespace Engine::Physics
{
    //Rect Collisions


    //TODO: Box Collisions for OBB
    //Box Collisions (AABB)
    bool Intersects(const Box& box, const Vector3f& point);
    bool Intersects(const Box& a, const Box& b);
    bool Intersects(const Box& box, const Sphere& sphere);
    bool Intersects(const Box& box, const Ray& ray);
    bool Intersects(const Box& box, const Frustrum& frustrum);

    //Sphere Collisions
    bool Intersects(const Sphere& a, const Sphere& b);
    bool Intersects(const Sphere& sphere, const Box& box);
    bool Intersects(const Sphere& sphere, const Ray& ray);
    bool Intersects(const Sphere& sphere, const Frustrum& frustrum);

}