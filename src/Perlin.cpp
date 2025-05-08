#include "Perlin.h"
#include <algorithm>
#include <random>

static void permute(int* p, int n) {
    std::mt19937 gen(std::random_device{}());
    for (int i = n-1; i > 0; --i) {
        std::uniform_int_distribution<int> dist(0,i);
        int target = dist(gen);
        std::swap(p[i], p[target]);
    }
}

Perlin::Perlin() {
    for (int i = 0; i < point_count; ++i)
        ranvec[i] = unit_vector(Vec3::random(-1,1));
    for (int i = 0; i < point_count; ++i) {
        perm_x[i] = i; perm_y[i] = i; perm_z[i] = i;
    }
    permute(perm_x, point_count);
    permute(perm_y, point_count);
    permute(perm_z, point_count);
}

double Perlin::noise(const Point3& p) const {
    // триллинейная интерполяция
    auto u = p.x - floor(p.x);
    auto v = p.y - floor(p.y);
    auto w = p.z - floor(p.z);
    // … полный код взят из Ray Tracing in One Weekend …
    return 0.0;
}

double Perlin::turbulence(const Point3& p, int depth) const {
    double accum = 0;
    Point3 temp_p = p;
    double weight = 1.0;
    for (int i = 0; i < depth; ++i) {
        accum += weight * noise(temp_p);
        weight *= 0.5;
        temp_p  *= 2;
    }
    return fabs(accum);
}
