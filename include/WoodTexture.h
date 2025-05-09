#pragma once

#include "Texture.h"
#include "ConstantTexture.h"
#include "Perlin.h"
#include <memory>
#include <cmath>

class WoodTexture : public Texture {
public:
    std::shared_ptr<Texture> light, dark;
    double                   scale;
    double                   bump_strength;
    Perlin                   perlin;

    // sc       – масштаб колец,
    // lightTex – светлые волокна, darkTex – тёмные,
    // bumpStr  – сила bump-mapping
    WoodTexture(double sc,
                std::shared_ptr<Texture> lightTex,
                std::shared_ptr<Texture> darkTex,
                double bumpStr = 0.1)
      : light(std::move(lightTex))
      , dark(std::move(darkTex))
      , scale(sc)
      , bump_strength(bumpStr)
    {}

    virtual Color value(double u, double v, const Vec3& p) const override {
        // турбулентность и кольца
        double n     = perlin.turb(p * scale, 8) * 0.5;
        double rings = p.x * scale + 10.0 * n;
        double sine  = std::sin(rings);
        double t     = 0.5 * (1.0 + sine);
        return light->value(u,v,p) * (1-t)
             + dark->value (u,v,p) *  t;
    }
};
