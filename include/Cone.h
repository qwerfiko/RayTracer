#pragma once

#include "Hittable.h"
#include "AABB.h"
#include <memory>
#include <cmath>

class Cone : public Hittable {
public:
    Point3 apex;      // вершина
    Vec3   axis;      // направление от вершины к основанию (должен быть unit_vector)
    double half_angle; // угол при вершине (в радианах)
    double height;    // высота от вершины до плоскости основания
    std::shared_ptr<Material> mat_ptr;

    Cone(
        const Point3&    apex_,
        const Vec3&      axis_,
        double           half_angle_,
        double           height_,
        std::shared_ptr<Material> m
    ) : apex(apex_),
        axis(unit_vector(axis_)),
        half_angle(half_angle_),
        height(height_),
        mat_ptr(m)
    {}

    virtual bool hit(
        const Ray& r,
        double t_min,
        double t_max,
        HitRecord& rec
    ) const override;

    virtual bool bounding_box(
        double time0,
        double time1,
        AABB& output_box
    ) const override;
};
