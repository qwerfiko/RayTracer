// include/Texture.h
#pragma once

#include "Vec3.h"

// Базовый класс текстур
class Texture {
public:
    // возвращает цвет по координатам (u,v) и по месту попадания p
    virtual Color value(double u, double v, const Point3& p) const = 0;
    virtual ~Texture() = default;
};
