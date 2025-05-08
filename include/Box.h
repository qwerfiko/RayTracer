#pragma once
#include "Hittable.h"
#include "AABB.h"

class Box : public Hittable {
public:
    Point3 box_min, box_max;
    std::shared_ptr<Material> mat_ptr;

    Box() {}
    Box(const Point3& p0, const Point3& p1, std::shared_ptr<Material> m)
      : box_min(p0), box_max(p1), mat_ptr(m) {}

    virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override;
    virtual bool bounding_box(double, double, AABB& output_box) const override {
        output_box = AABB(box_min, box_max);
        return true;
    }
};
