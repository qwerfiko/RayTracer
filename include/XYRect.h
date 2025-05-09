#pragma once
#include <memory>
#include "Hittable.h"
#include "AABB.h"

class XYRect : public Hittable {
public:
    double x0, x1, y0, y1, k;
    std::shared_ptr<Material> mp;

    XYRect() {}
    XYRect(double _x0, double _x1, double _y0, double _y1, double _k,
           std::shared_ptr<Material> mat)
      : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {}

    virtual bool hit(const Ray& r, double t0, double t1, HitRecord& rec) const override {
        auto t = (k - r.origin.z) / r.direction.z;
        if (t < t0 || t > t1) return false;
        auto x = r.origin.x + t*r.direction.x;
        auto y = r.origin.y + t*r.direction.y;
        if (x < x0 || x > x1 || y < y0 || y > y1) return false;
        rec.u = (x - x0)/(x1 - x0);
        rec.v = (y - y0)/(y1 - y0);
        rec.t = t;
        rec.mat_ptr = mp;
        rec.p = r.at(t);
        rec.set_face_normal(r, Vec3(0,0,1));
        return true;
    }

    virtual bool bounding_box(double, double, AABB& box) const override {
        // добавить толщину по z
        box = AABB(Point3(x0,y0,k-0.0001), Point3(x1,y1,k+0.0001));
        return true;
    }
};
