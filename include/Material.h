#pragma once

#include "Ray.h"
#include "Hittable.h"
#include "Texture.h"
#include <memory>
#include "Vec3.h"

// утилиты для преломления/отражения
inline Vec3 reflect(const Vec3& v, const Vec3& n) {
    return v - 2*dot(v,n)*n;
}

inline Vec3 refract(const Vec3& uv, const Vec3& n, double etai_over_etat) {
    double cos_theta = dot(-uv, n);
    Vec3 r_out_perp =  etai_over_etat * (uv + cos_theta*n);
    Vec3 r_out_parallel = -std::sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel;
}

// Заполняет информацию о рассеянном луче
struct ScatterRecord {
    Ray       specular_ray;
    bool      is_specular;
    Color     attenuation;
};

class Material {
public:
    // scatter возвращает true, если луч рассеялся / отражён / преломлён
    // заполняет srec
    virtual bool scatter(
        const Ray& r_in,
        const HitRecord& rec,
        ScatterRecord& srec
    ) const = 0;

    // эмиссия (для источников света). По умолчанию — ноль.
    virtual Color emitted() const { return Color(0,0,0); }

    virtual ~Material() = default;
};

class Lambertian : public Material {
public:
    // albedo хранит текстуру (например, ConstantTexture или WoodTexture)
    std::shared_ptr<Texture> albedo;

    explicit Lambertian(std::shared_ptr<Texture> a);

    virtual bool scatter(
        const Ray& r_in,
        const HitRecord& rec,
        ScatterRecord& srec
    ) const override;
};

class Metal : public Material {
public:
    Color albedo;
    double fuzz;
    Metal(const Color& a, double f);
    virtual bool scatter(
        const Ray& r_in,
        const HitRecord& rec,
        ScatterRecord& srec
    ) const override;
};

class Dielectric : public Material {
public:
    explicit Dielectric(double index_of_refraction);

    // This must match the base class exactly:
    virtual bool scatter(
        const Ray& r_in,
        const HitRecord& rec,
        ScatterRecord& srec
    ) const override;

private:
    double ir;
    static double reflectance(double cosine, double ref_idx);
};


class DiffuseLight : public Material {
public:
    std::shared_ptr<Texture> emit;
    explicit DiffuseLight(std::shared_ptr<Texture> a);
    virtual bool scatter(
        const Ray& r_in,
        const HitRecord& rec,
        ScatterRecord& srec
    ) const override { return false; }
    virtual Color emitted() const override;
};
