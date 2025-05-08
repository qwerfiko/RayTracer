#pragma once

#include <memory>
#include "Ray.h"
#include "Hittable.h"
#include "Texture.h"

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
    std::shared_ptr<Texture> albedo;
    Lambertian(std::shared_ptr<Texture> a) : albedo(a) {}

    virtual bool scatter(
        const Ray& r_in, const HitRecord& rec, ScatterRecord& srec
    ) const override {
        auto scatter_direction = rec.normal + Vec3::random(-1,1);
        if (scatter_direction.length_squared() < 1e-8)
            scatter_direction = rec.normal;
        srec.specular_ray = Ray(rec.p, unit_vector(scatter_direction));
        srec.attenuation  = albedo->value(rec.u, rec.v, rec.p);
        srec.is_specular  = false;
        return true;
    }
};



class Metal : public Material {
public:
    Vec3 albedo;
    double fuzz;
    Metal(const Vec3&, double);

    virtual bool scatter(
        const Ray& r_in,
        const HitRecord& rec,
        ScatterRecord& srec
    ) const override;
};


class Dielectric : public Material {
public:
    double ir;
    Dielectric(double);
        virtual bool scatter(
        const Ray& r_in,
        const HitRecord& rec,
        ScatterRecord& srec
    ) const override;
private:
    static double reflectance(double, double);
};


// Экранно-диффузный источник света
class DiffuseLight : public Material {
public:
    Color emit;
    DiffuseLight(const Color&);
        virtual bool scatter(
        const Ray& r_in,
        const HitRecord& rec,
        ScatterRecord& srec
    ) const override;
    virtual Color emitted() const override { return emit; }
};
