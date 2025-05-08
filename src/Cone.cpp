// src/Cone.cpp

#include "Cone.h"
#include "Vec3.h"
#include <cmath>
#include <algorithm>

// Helper: build an orthonormal basis (u,v,w) with w = axis
static void build_orthonormal_basis(const Vec3& w, Vec3& u, Vec3& v) {
    Vec3 a = (std::fabs(w.x) > 0.9) ? Vec3(0,1,0) : Vec3(1,0,0);
    u = unit_vector(cross(a, w));
    v = cross(w, u);
}

bool Cone::hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const {
    // 1) Build local frame
    Vec3 w = axis;  // already unit
    Vec3 u, v;
    build_orthonormal_basis(w, u, v);

    // 2) Translate ray origin so apex is at (0,0,0)
    Vec3 co = r.origin - apex;

    // 3) Express ray in local coordinates of cone
    double Dx = dot(r.direction, u);
    double Dy = dot(r.direction, w);
    double Dz = dot(r.direction, v);

    double Ox = dot(co, u);
    double Oy = dot(co, w);
    double Oz = dot(co, v);

    // 4) Cone equation: x^2 + z^2 - (tan(angle))^2 y^2 = 0
    double k    = std::tan(angle);
    double k2   = k * k;

    double A = Dx*Dx + Dz*Dz - k2 * (Dy*Dy);
    double B = 2*(Ox*Dx + Oz*Dz - k2 * (Oy*Dy));
    double C = Ox*Ox + Oz*Oz - k2 * (Oy*Oy);

    // 5) Solve quadratic At^2 + Bt + C = 0
    double disc = B*B - 4*A*C;
    if (disc < 0) return false;
    double sqrtd = std::sqrt(disc);

    // Two candidate t's
    double t0 = (-B - sqrtd) / (2*A);
    double t1 = (-B + sqrtd) / (2*A);

    // Pick the nearest valid root in [t_min, t_max]
    double t = t0;
    if (t < t_min || t > t_max) {
        t = t1;
        if (t < t_min || t > t_max)
            return false;
    }

    // 6) Check that the hit lies between apex (y=0) and base (y=height)
    double y = Oy + t*Dy;
    if (y < 0 || y > height)
        return false;

    // 7) Fill hit record
    rec.t = t;
    rec.p = r.at(t);
    rec.mat_ptr = mat_ptr;

    // 8) Compute normal: gradient of F(x,y,z)=x^2+z^2-k^2 y^2 is (2x, -2k^2 y, 2z)
    //    in local coords at P' = (Ox+t*Dx, Oy+t*Dy, Oz+t*Dz)
    Vec3 P_local(Ox + t*Dx, y, Oz + t*Dz);
    Vec3 N_local(P_local.x, -k2 * P_local.y, P_local.z);

    // Transform normal back to world space
    Vec3 normal = unit_vector(u * N_local.x + w * N_local.y + v * N_local.z);
    rec.set_face_normal(r, normal);

    return true;
}
