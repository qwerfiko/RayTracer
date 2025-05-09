#pragma once
#include "Vec3.h"
#include <array>
#include <numeric>
#include <random>

class Perlin {
public:
    Perlin();
    double noise(const Point3& p) const;
    double turb(const Point3& p, int depth=7) const;

private:
    static const int pointCount = 256;
    std::array<int, pointCount*2> perm;

    static std::array<int, pointCount> generate_perm();
    static double fade(double t);
    static double lerp(double t, double a, double b);
    static double grad(int hash, double x, double y, double z);
};
