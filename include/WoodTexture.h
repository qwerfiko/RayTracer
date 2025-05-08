#pragma once
#include "Texture.h"
#include <cmath>

class WoodTexture : public Texture {
public:
    Perlin noise;
    double scale;
    WoodTexture(double sc = 20.0) : scale(sc) {}

    virtual Color value(double u, double v, const Point3& p) const override {
        double n = noise.turbulence(p * scale);
        double rings = p.x*scale + n;
        double t = fmod(rings, 2.0);
        return (t < 1.0)
             ? Color(0.4,0.2,0.1)
             : Color(0.3,0.15,0.05);
    }
};
