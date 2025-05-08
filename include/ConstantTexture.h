// include/ConstantTexture.h
#pragma once

#include "Texture.h"
#include "Vec3.h"

// Текстура, возвращающая всегда один и тот же цвет
class ConstantTexture : public Texture {
public:
    Color color;
    ConstantTexture(const Color& c) : color(c) {}
    virtual Color value(double /*u*/, double /*v*/, const Point3& /*p*/) const override {
        return color;
    }
};
