#include "../inc/Core/Physics.h"

bool Engine::Physics::Intersects(const Box& box, const Vector3f& point)
{
    if ((point.x < box.Left()) || (point.x > box.Right()))
    {
        return false;
    }
    if ((point.y < box.Bottom()) || (point.y > box.Top()))
    {
        return false;
    }
    if ((point.z < box.Back() || point.x > box.Front()))
    {
        return false;
    }

    return true;
}

bool Engine::Physics::Intersects(const Box& box, const Ray& ray)
{
    return true;
}
