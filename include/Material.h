#pragma once

#include "Ray.h"
#include "Hittable.h"
#include "Vec3.h"

struct ScatterRecord {
    Ray specular_ray;
    bool is_specular;
    Color attenuation;
};

class Material {
public:
    virtual ~Material() = default;
    virtual bool scatter(
        const Ray& r_in,
        const HitRecord& rec,
        ScatterRecord& srec
    ) const = 0;
    virtual Color emitted() const { return Color(0,0,0); }
};

// Lambertian (диффузное)
class Lambertian : public Material {
public:
    explicit Lambertian(const Color& a);

    bool scatter(
        const Ray& r_in,
        const HitRecord& rec,
        ScatterRecord& srec
    ) const override;

private:
    Color albedo;
};

// Metal (отражающее)
class Metal : public Material {
public:
    Metal(const Color& a, double f);

    bool scatter(
        const Ray& r_in,
        const HitRecord& rec,
        ScatterRecord& srec
    ) const override;

private:
    Color albedo;
    double fuzz;
};

// Dielectric (преломляющее стекло)
class Dielectric : public Material {
public:
    explicit Dielectric(double index_of_refraction);

    bool scatter(
        const Ray& r_in,
        const HitRecord& rec,
        ScatterRecord& srec
    ) const override;

private:
    double ir;  // index of refraction

    // Вспомогательные функции — будут определены в Material.cpp
    static double reflectance(double cosine, double ref_idx);
    static Vec3 refract(const Vec3& uv, const Vec3& n, double etai_over_etat);
};

// Экранно-диффузный источник света
class DiffuseLight : public Material {
public:
    Color emit;  // собственное излучение

    DiffuseLight(const Color& c) : emit(c) {}

    // не рассеиваем луч — он поглощается и даёт только emission
    virtual bool scatter(
        const Ray& r_in, const HitRecord& rec, ScatterRecord& srec
    ) const override {
        return false;
    }

    // по умолчанию scatter==false означает: возвращаем emit
    virtual Color emitted() const {
        return emit;
    }
};
