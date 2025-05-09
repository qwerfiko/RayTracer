// Cone.h
#pragma once
#include "Hittable.h"
#include <memory>

class Cone : public Hittable {
public:
    Point3 apex;
    Vec3   axis;   // единичный вектор вдоль оси конуса
    double height;
    double radius;
    std::shared_ptr<Material> mat_ptr;

    Cone(
      const Point3& a,
      const Vec3&   axis_dir,
      double        h,
      double        r,
      std::shared_ptr<Material> m
    ) : apex(a),
        axis(unit_vector(axis_dir)),
        height(h),
        radius(r),
        mat_ptr(std::move(m))
    {}

    virtual bool hit(
      const Ray& r, double t_min, double t_max, HitRecord& rec
    ) const override;

    virtual bool bounding_box(
      double time0, double time1, AABB& output_box
    ) const override;
};
