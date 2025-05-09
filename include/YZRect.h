#pragma once
#include <memory>
#include "Hittable.h"
#include "AABB.h"

class YZRect : public Hittable {
public:
    double y0, y1, z0, z1, k;
    std::shared_ptr<Material> mp;

    YZRect() {}
    YZRect(double _y0, double _y1, double _z0, double _z1, double _k,
           std::shared_ptr<Material> mat)
      : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {}

    virtual bool hit(const Ray& r, double t0, double t1, HitRecord& rec) const override {
        auto t = (k - r.origin.x) / r.direction.x;
        if (t < t0 || t > t1) return false;
        auto y = r.origin.y + t*r.direction.y;
        auto z = r.origin.z + t*r.direction.z;
        if (y < y0 || y > y1 || z < z0 || z > z1) return false;
        rec.u = (y - y0)/(y1 - y0);
        rec.v = (z - z0)/(z1 - z0);
        rec.t = t;
        rec.mat_ptr = mp;
        rec.p = r.at(t);
        rec.set_face_normal(r, Vec3(1,0,0));
        return true;
    }

    virtual bool bounding_box(double, double, AABB& box) const override {
        box = AABB(Point3(k-0.0001,y0,z0), Point3(k+0.0001,y1,z1));
        return true;
    }
};
