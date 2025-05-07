#include "AABB.h"
#include <algorithm> // std::swap
#include <cmath>     // std::fmin, std::fmax

AABB::AABB() : minimum(Point3(0,0,0)), maximum(Point3(0,0,0)) {}
AABB::AABB(const Point3& a, const Point3& b) : minimum(a), maximum(b) {}

Point3 AABB::min() const { return minimum; }
Point3 AABB::max() const { return maximum; }

bool AABB::hit(const Ray& r, double t_min, double t_max) const {
    for (int a = 0; a < 3; ++a) {
        double invD = 1.0 / r.direction[a];
        double t0   = (minimum[a] - r.origin[a]) * invD;
        double t1   = (maximum[a] - r.origin[a]) * invD;
        if (invD < 0.0) std::swap(t0, t1);
        t_min = t0 > t_min ? t0 : t_min;
        t_max = t1 < t_max ? t1 : t_max;
        if (t_max <= t_min)
            return false;
    }
    return true;
}

AABB AABB::surrounding_box(const AABB& box0, const AABB& box1) {
    Point3 small(
        std::fmin(box0.min().x, box1.min().x),
        std::fmin(box0.min().y, box1.min().y),
        std::fmin(box0.min().z, box1.min().z)
    );
    Point3 big(
        std::fmax(box0.max().x, box1.max().x),
        std::fmax(box0.max().y, box1.max().y),
        std::fmax(box0.max().z, box1.max().z)
    );
    return AABB(small, big);
}
