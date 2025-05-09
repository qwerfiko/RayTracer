#pragma once

#include "Hittable.h"
#include "AABB.h"
#include <memory>

class Sphere : public Hittable {
public:
    Point3 center;
    double radius;
    std::shared_ptr<Material> mat_ptr;

    Sphere();
    Sphere(Point3 cen, double r, std::shared_ptr<Material> m);


    bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const override;
    bool bounding_box(double time0, double time1, AABB& output_box) const override;
};
