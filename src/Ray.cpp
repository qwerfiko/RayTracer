#include "Ray.h"

Ray::Ray() = default;

Ray::Ray(const Point3& origin, const Vec3& direction)
    : origin(origin), direction(direction)
{}

Point3 Ray::at(double t) const {
    return origin + t * direction;
}
