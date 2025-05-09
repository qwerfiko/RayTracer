// Axis-Aligned Bounding Box: проверка пересечений и объединение двух коробок.
//
#pragma once

#include "Vec3.h"
#include "Ray.h"

class AABB {
public:
    AABB();
    AABB(const Point3& a, const Point3& b);

    Point3 min() const;
    Point3 max() const;

    bool hit(const Ray& r, double t_min, double t_max) const;
    static AABB surrounding_box(const AABB& box0, const AABB& box1);

    Point3 minimum;
    Point3 maximum;
};
