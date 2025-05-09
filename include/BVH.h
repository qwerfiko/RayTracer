// Строит бинарное дерево по объектам для быстрого отсечения множества объекто
#pragma once

#include "Hittable.h"
#include <vector>

/**
 * @brief Ускоряющая структура BVH-узел для коллекции объектов.
 */

class BVHNode : public Hittable {
public:
    BVHNode();
    /**
     * @param src_objects  исходный массив объектов
     * @param start        индекс начала диапазона (включительно)
     * @param end          индекс конца диапазона (не включительно)
     * @param time0        начало интервала времени
     * @param time1        конец интервала времени
     */
    BVHNode(
        const std::vector<HittablePtr>& src_objects,
        size_t start, size_t end,
        double time0, double time1
    );

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

private:
    HittablePtr left;
    HittablePtr right;
    AABB        box;
};

/**
 * @brief Компаратор по оси X для сортировки объектов при построении BVH.
 */
bool box_x_compare(const HittablePtr a, const HittablePtr b);

/**
 * @brief Компаратор по оси Y для сортировки объектов при построении BVH.
 */
bool box_y_compare(const HittablePtr a, const HittablePtr b);

/**
 * @brief Компаратор по оси Z для сортировки объектов при построении BVH.
 */
bool box_z_compare(const HittablePtr a, const HittablePtr b);
