#include "Sphere.h"
#include <cmath>

Sphere::Sphere()
    : center(Point3(0,0,0)), radius(0), mat_ptr(nullptr)
{}

Sphere::Sphere(Point3 cen, double r, std::shared_ptr<Material> m)
    : center(cen), radius(r), mat_ptr(m)
{}

bool Sphere::hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const {
    Vec3 oc = r.origin - center;
    double a = r.direction.length_squared();
    double half_b = dot(oc, r.direction);
    double c = oc.length_squared() - radius*radius;
    double discriminant = half_b*half_b - a*c;
    if (discriminant < 0) return false;
    double sqrtd = std::sqrt(discriminant);

    double root = (-half_b - sqrtd) / a;
    if (root < t_min || root > t_max) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || root > t_max)
            return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    Vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    rec.mat_ptr = mat_ptr;

    return true;
}

bool Sphere::bounding_box(double time0, double time1, AABB& output_box) const {
    output_box = AABB(
        center - Vec3(radius, radius, radius),
        center + Vec3(radius, radius, radius)
    );
    return true;
}
