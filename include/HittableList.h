#pragma once

#include <vector>
#include "Hittable.h"

/**
 * @brief Список объектов для рейтрейсера.
 */
class HittableList : public Hittable {
public:
    HittableList();
    explicit HittableList(HittablePtr object);

    void clear();
    void add(HittablePtr object);

    bool hit(
        const Ray& r,
        double t_min,
        double t_max,
        HitRecord& rec
    ) const override;

    bool bounding_box(
        double time0,
        double time1,
        AABB& output_box
    ) const override;

    std::vector<HittablePtr> objects;
};
