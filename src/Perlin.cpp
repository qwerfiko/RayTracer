#include "Perlin.h"
#include <algorithm>

Perlin::Perlin() {
    auto p = generate_perm();
    for (int i = 0; i < pointCount; ++i)
        perm[i] = perm[i+pointCount] = p[i];
}

std::array<int, Perlin::pointCount> Perlin::generate_perm() {
    std::array<int, pointCount> p;
    std::iota(p.begin(), p.end(), 0);
    std::mt19937 gen(std::random_device{}());
    std::shuffle(p.begin(), p.end(), gen);
    return p;
}

double Perlin::noise(const Point3& p) const {
    auto x = p.x - std::floor(p.x);
    auto y = p.y - std::floor(p.y);
    auto z = p.z - std::floor(p.z);
    int xi = (int)std::floor(p.x) & 255;
    int yi = (int)std::floor(p.y) & 255;
    int zi = (int)std::floor(p.z) & 255;

    double u = fade(x), v = fade(y), w = fade(z);
    int A  = perm[xi]   + yi, AA = perm[A]   + zi, AB = perm[A+1] + zi;
    int B  = perm[xi+1] + yi, BA = perm[B]   + zi, BB = perm[B+1] + zi;

    double res = lerp(w,
        lerp(v,
             lerp(u, grad(perm[AA],   x,   y,   z),
                      grad(perm[BA],   x-1, y,   z)),
             lerp(u, grad(perm[AB],   x,   y-1, z),
                      grad(perm[BB],   x-1, y-1, z))),
        lerp(v,
             lerp(u, grad(perm[AA+1], x,   y,   z-1),
                      grad(perm[BA+1], x-1, y,   z-1)),
             lerp(u, grad(perm[AB+1], x,   y-1, z-1),
                      grad(perm[BB+1], x-1, y-1, z-1)))
    );
    return (res + 1.0) / 2.0;
}

double Perlin::turb(const Point3& p, int depth) const {
    double accum = 0;
    Point3  temp = p;
    double weight = 1.0;
    for (int i = 0; i < depth; ++i) {
        accum    += weight * noise(temp);
        weight  *= 0.5;
        temp     = temp * 2;
    }
    return accum;
}

double Perlin::fade(double t)    { return t*t*t*(t*(t*6-15)+10); }
double Perlin::lerp(double t, double a, double b) { return a + t*(b-a); }
double Perlin::grad(int hash, double x, double y, double z) {
    int h = hash & 15;
    double u = h<8 ? x : y;
    double v = h<4 ? y : (h==12||h==14 ? x : z);
    return ((h&1)? -u : u) + ((h&2)? -v : v);
}
