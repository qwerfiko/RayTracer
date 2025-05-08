#pragma once
#include <vector>
#include "Vec3.h"

class Perlin {
public:
    static const int point_count = 256;
    Vec3 ranvec[point_count];
    int perm_x[point_count], perm_y[point_count], perm_z[point_count];

    Perlin();
    double noise(const Point3& p) const;
    double turbulence(const Point3& p, int depth=7) const;
};
