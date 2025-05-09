#pragma once
#include "Texture.h"
#include "Perlin.h"
#include <cmath>

class NoiseTexture : public Texture {
public:
    Perlin noise;
    double scale;
    NoiseTexture(double sc = 1.0) : scale(sc) {}
    virtual Color value(double u, double v, const Point3& p) const override {
        // например, вейвлет-эффект
        double t = 0.5*(1 + sin(scale*p.z + 10*noise.turb(p)));
        return Color(1,1,1) * t;
    }
};
