#include "Camera.h"
#include <cmath>
#include <random>

namespace {
    // Потокобезопасный RNG
    static thread_local std::mt19937 gen(std::random_device{}());

    // Случайное число в диапазоне [min, max)
    inline double random_double(double min, double max) {
        std::uniform_real_distribution<double> dist(min, max);
        return dist(gen);
    }
}

Camera::Camera(
    Point3 lookfrom,
    Point3 lookat,
    Vec3   vup,
    double vfov,
    double aspect,
    double aperture,
    double focus_dist
) {
    double theta = degrees_to_radians(vfov);
    double h     = std::tan(theta / 2.0);
    double viewport_height = 2.0 * h;
    double viewport_width  = aspect * viewport_height;

    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);

    origin             = lookfrom;
    horizontal         = focus_dist * viewport_width  * u;
    vertical           = focus_dist * viewport_height * v;
    lower_left_corner  = origin
                         - horizontal / 2
                         - vertical   / 2
                         - focus_dist * w;

    lens_radius = aperture / 2.0;
}

Ray Camera::get_ray(double s, double t) const {
    Vec3 rd     = lens_radius * random_in_unit_disk();
    Vec3 offset = u * rd.x + v * rd.y;
    return Ray(
        origin + offset,
        lower_left_corner
          + s * horizontal
          + t * vertical
          - origin
          - offset
    );
}

double Camera::degrees_to_radians(double degrees) {
    return degrees * M_PI / 180.0;
}

Vec3 Camera::random_in_unit_disk() {
    while (true) {
        double x = random_double(-1.0, 1.0);
        double y = random_double(-1.0, 1.0);
        Vec3 p(x, y, 0.0);
        if (p.length_squared() >= 1.0) continue;
        return p;
    }
}
