#include "Material.h"

#include <random>
#include <cmath>

namespace {
    // Потокобезопасный RNG для scattering
    static thread_local std::mt19937 gen(std::random_device{}());
    static inline double rand01() {
        static std::uniform_real_distribution<double> dist(0.0, 1.0);
        return dist(gen);
    }

    // Утилита отражения
    static inline Vec3 reflect(const Vec3& v, const Vec3& n) {
        return v - 2 * dot(v, n) * n;
    }
}

Lambertian::Lambertian(const Color& a)
    : albedo(a)
{}

bool Lambertian::scatter(
    const Ray& r_in,
    const HitRecord& rec,
    ScatterRecord& srec
) const {
    Vec3 scatter_direction = rec.normal + Vec3::random(-1, 1);
    if (scatter_direction.length_squared() < 1e-8)
        scatter_direction = rec.normal;

    srec.specular_ray = Ray(rec.p, unit_vector(scatter_direction));
    srec.attenuation  = albedo;
    srec.is_specular  = false;
    return true;
}

Metal::Metal(const Color& a, double f)
    : albedo(a), fuzz(f < 1 ? f : 1)
{}

bool Metal::scatter(
    const Ray& r_in,
    const HitRecord& rec,
    ScatterRecord& srec
) const {
    Vec3 reflected = reflect(unit_vector(r_in.direction), rec.normal);
    srec.specular_ray = Ray(rec.p, reflected + fuzz * Vec3::random());
    srec.attenuation  = albedo;
    srec.is_specular  = true;
    return true;
}

Dielectric::Dielectric(double index_of_refraction)
    : ir(index_of_refraction)
{}

bool Dielectric::scatter(
    const Ray& r_in,
    const HitRecord& rec,
    ScatterRecord& srec
) const {
    srec.attenuation = Color(1.0, 1.0, 1.0);
    srec.is_specular = true;

    double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;
    Vec3 unit_direction    = unit_vector(r_in.direction);

    double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
    Vec3 direction;

    if (cannot_refract ||
        reflectance(cos_theta, refraction_ratio) > rand01())
    {
        direction = reflect(unit_direction, rec.normal);
    } else {
        direction = refract(unit_direction, rec.normal, refraction_ratio);
    }

    srec.specular_ray = Ray(rec.p, direction);
    return true;
}

double Dielectric::reflectance(double cosine, double ref_idx) {
    double r0 = (1.0 - ref_idx) / (1.0 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1.0 - r0) * std::pow((1.0 - cosine), 5.0);
}

Vec3 Dielectric::refract(const Vec3& uv, const Vec3& n, double etai_over_etat) {
    double cos_theta = dot(-uv, n);
    Vec3  r_out_perp = etai_over_etat * (uv + cos_theta * n);
    Vec3  r_out_parallel =
        -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}
