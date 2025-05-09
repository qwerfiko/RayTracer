#include "Material.h"
#include "WoodTexture.h"
#include <cmath>
#include <random>

// ---- Lambertian ----

Lambertian::Lambertian(std::shared_ptr<Texture> a)
  : albedo(std::move(a))
{}

bool Lambertian::scatter(
    const Ray& r_in,
    const HitRecord& rec,
    ScatterRecord& srec
) const {
    Vec3 N = rec.normal;

    // bump-mapping для WoodTexture
    if (auto wt = dynamic_cast<const WoodTexture*>(albedo.get())) {
        const Perlin& noise = wt->perlin;
        const double eps   = 1e-3;
        double h0 = noise.turb(rec.p);
        double hx = noise.turb(rec.p + Vec3(eps,0,0));
        double hy = noise.turb(rec.p + Vec3(0,eps,0));
        double hz = noise.turb(rec.p + Vec3(0,0,eps));
        Vec3 grad((hx-h0)/eps, (hy-h0)/eps, (hz-h0)/eps);
        N = unit_vector(N + wt->bump_strength * grad);
    }

    // Рассеиваем в случайном направлении вокруг N
    Vec3 scatter_direction = N + Vec3::random(-1,1);
    if (scatter_direction.length_squared() < 1e-8)
        scatter_direction = N;

    srec.specular_ray = Ray(rec.p, unit_vector(scatter_direction));
    srec.attenuation  = albedo->value(rec.u, rec.v, rec.p);
    srec.is_specular  = false;
    return true;
}

// ---- Metal ----

Metal::Metal(const Color& a, double f)
  : albedo(a), fuzz(f < 1? f : 1)
{}

bool Metal::scatter(
    const Ray& r_in,
    const HitRecord& rec,
    ScatterRecord& srec
) const {
    Vec3 reflected = reflect(unit_vector(r_in.direction), rec.normal);
    srec.specular_ray = Ray(rec.p, reflected + fuzz*Vec3::random());
    srec.attenuation  = albedo;
    srec.is_specular  = true;
    return (dot(srec.specular_ray.direction, rec.normal) > 0);
}

// ---- Dielectric ----

Dielectric::Dielectric(double index_of_refraction)
  : ir(index_of_refraction)
{}

bool Dielectric::scatter(
    const Ray& r_in,
    const HitRecord& rec,
    ScatterRecord& srec
) const {
    srec.attenuation = Color(1,1,1);
    srec.is_specular = true;
    double refraction_ratio = rec.front_face ? (1.0/ir) : ir;
    Vec3 unit_dir = unit_vector(r_in.direction);
    double cos_theta = std::fmin(dot(-unit_dir, rec.normal), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);
    bool cannot_refract = refraction_ratio * sin_theta > 1.0;
    Vec3 direction;
    if (cannot_refract || Dielectric::reflectance(cos_theta, refraction_ratio) > Vec3::random().x)
        direction = reflect(unit_dir, rec.normal);
    else
        direction = refract(unit_dir, rec.normal, refraction_ratio);
    srec.specular_ray = Ray(rec.p, direction);
    return true;
}

double Dielectric::reflectance(double cosine, double ref_idx) {
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0)*std::pow((1 - cosine),5);
}

// ---- DiffuseLight ----

DiffuseLight::DiffuseLight(std::shared_ptr<Texture> a)
  : emit(std::move(a))
{}

Color DiffuseLight::emitted() const {
    return emit->value(0,0,Vec3());
