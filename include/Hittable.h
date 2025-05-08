#pragma once

#include "Ray.h"
#include "AABB.h"
#include <memory>

struct Material;   // впереди объявляем

struct HitRecord {
     Point3 p;
     Vec3 normal;
     std::shared_ptr<Material> mat_ptr;

    double t;
    double u, v;              // текстурные координаты
     bool front_face;

     inline void set_face_normal(const Ray& r, const Vec3& outward_normal) {
         front_face = dot(r.direction, outward_normal) < 0;
         normal = front_face ? outward_normal : -outward_normal;
     }
 };

class Hittable {
public:
    virtual ~Hittable() = default;

    // проверка пересечения луча с объектом
    virtual bool hit(
        const Ray& r,
        double t_min,
        double t_max,
        HitRecord& rec
    ) const = 0;

    // получение ограничивающей коробки в заданный интервал времени
    virtual bool bounding_box(
        double time0,
        double time1,
        AABB& output_box
    ) const = 0;
};

using HittablePtr = std::shared_ptr<Hittable>;
