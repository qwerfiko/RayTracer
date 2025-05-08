// src/Material.cpp

#include "Material.h"
#include "Vec3.h"    // для Vec3, dot(), unit_vector(), random_double()
#include <cmath>     // для std::sqrt, std::pow, std::fabs

// --- Утилиты для отражения и преломления (они не лежат в Vec3) ---

static Vec3 reflect(const Vec3& v, const Vec3& n) {
    return v - 2*dot(v,n)*n;
}

static Vec3 refract(const Vec3& uv, const Vec3& n, double etai_over_etat) {
    double cos_theta = dot(-uv, n);
    Vec3 r_out_perp   =  etai_over_etat * (uv + cos_theta*n);
    Vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

// --- Metal ---

Metal::Metal(const Vec3& a, double f)
  : albedo(a), fuzz(f < 1 ? f : 1) {}

bool Metal::scatter(
    const Ray& r_in,
    const HitRecord& rec,
    ScatterRecord& srec
) const {
    Vec3 reflected = reflect(unit_vector(r_in.direction), rec.normal);
    srec.specular_ray = Ray(rec.p, reflected + fuzz * Vec3::random());
    srec.attenuation  = albedo;
    srec.is_specular  = true;
    return dot(srec.specular_ray.direction, rec.normal) > 0;
}

// --- Dielectric (glass) ---

Dielectric::Dielectric(double index_of_refraction)
  : ir(index_of_refraction) {}

bool Dielectric::scatter(
    const Ray& r_in,
    const HitRecord& rec,
    ScatterRecord& srec
) const {
    srec.attenuation = Color(1.0, 1.0, 1.0);
    srec.is_specular = true;

    double refraction_ratio = rec.front_face ? (1.0/ir) : ir;
    Vec3 unit_dir = unit_vector(r_in.direction);

    double cos_theta = std::fmin(dot(-unit_dir, rec.normal), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
    Vec3 direction;
    if (cannot_refract
        || Dielectric::reflectance(cos_theta, refraction_ratio) > random_double())
    {
        direction = reflect(unit_dir, rec.normal);
    } else {
        direction = refract(unit_dir, rec.normal, refraction_ratio);
    }

    srec.specular_ray = Ray(rec.p, direction);
    return true;
}

double Dielectric::reflectance(double cosine, double ref_idx) {
    // Schlick’s approximation
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
}

// --- DiffuseLight (эмиттер) ---

DiffuseLight::DiffuseLight(const Color& c)
  : emit(c) {}

bool DiffuseLight::scatter(
    const Ray&,
    const HitRecord&,
    ScatterRecord&
) const {
    return false;
}

// emitted() реализован inline в Material.h
