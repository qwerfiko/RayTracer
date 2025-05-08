// include/Cone.h
#pragma once

#include <memory>
#include "Hittable.h"
#include "Vec3.h"
#include "Ray.h"
#include "AABB.h"
#include "Material.h"

class Cone : public Hittable {
public:
    Point3 apex;
    Vec3   axis;     // должен быть unit_vector
    double angle;    // apex half-angle в радианах
    double height;
    std::shared_ptr<Material> mat_ptr;

    Cone(const Point3& a, const Vec3& ax,
         double ang, double h,
         std::shared_ptr<Material> m)
      : apex(a), axis(unit_vector(ax)), angle(ang),
        height(h), mat_ptr(m) {}

    virtual bool hit(
        const Ray& r, double t_min, double t_max, HitRecord& rec
    ) const override;

    virtual bool bounding_box(
        double time0, double time1, AABB& output_box
    ) const override {
        // грубый AABB: расширяем в обоих направлениях
        Point3 rad = Point3(height * std::tan(angle),
                            height,
                            height * std::tan(angle));
        output_box = AABB(apex - rad, apex + rad);
        return true;
    }
};
