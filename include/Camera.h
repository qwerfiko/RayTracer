#pragma once

#include "Vec3.h"
#include "Ray.h"

/**
 * @brief Параметрическая камера с глубиной резкости.
 */
class Camera {
public:
    Point3 origin;
    Point3 lower_left_corner;
    Vec3   horizontal;
    Vec3   vertical;
    Vec3   u, v, w;
    double lens_radius;

    /**
     * @param lookfrom  точка, откуда смотрим
     * @param lookat    точка, на которую смотрим
     * @param vup       “верх” в мировых координатах
     * @param vfov      вертикальный угол обзора в градусах
     * @param aspect    соотношение сторон (width/height)
     * @param aperture  апертура (диаметр объектива)
     * @param focus_dist  дистанция фокуса
     */
    Camera(
        Point3 lookfrom,
        Point3 lookat,
        Vec3   vup,
        double vfov,
        double aspect,
        double aperture,
        double focus_dist
    );

    /**
     * @brief Сгенерировать луч, проходящий через точку (s,t) на экране.
     */
    Ray get_ray(double s, double t) const;

private:
    static double degrees_to_radians(double degrees);
    static Vec3   random_in_unit_disk();
};
