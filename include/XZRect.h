// include/XZRect.h
#pragma once
#include <memory>
#include "Hittable.h"
#include "AABB.h"

class XZRect : public Hittable {
public:
    double x0, x1, z0, z1, k;
    std::shared_ptr<Material> mp;

    XZRect() {}
    XZRect(double _x0, double _x1, double _z0, double _z1, double _k,
           std::shared_ptr<Material> mat)
      : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {}

    virtual bool hit(const Ray& r, double t0, double t1, HitRecord& rec) const override {
        auto t = (k - r.origin.y) / r.direction.y;
        if (t < t0 || t > t1) return false;
        auto x = r.origin.x + t*r.direction.x;
        auto z = r.origin.z + t*r.direction.z;
        if (x < x0 || x > x1 || z < z0 || z > z1) return false;
        rec.u = (x - x0)/(x1 - x0);
        rec.v = (z - z0)/(z1 - z0);
        rec.t = t;
        rec.mat_ptr = mp;
        rec.p = r.at(t);
        rec.set_face_normal(r, Vec3(0,1,0));
        return true;
    }

    virtual bool bounding_box(double, double, AABB& box) const override {
        box = AABB(Point3(x0,k-0.0001,z0), Point3(x1,k+0.0001,z1));
        return true;
    }
};
