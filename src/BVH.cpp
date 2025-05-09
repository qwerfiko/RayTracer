#include "BVH.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>

namespace {
    // Вспомогательная функция: сравнить две AABB по координате axis
    bool box_compare_axis(
        const HittablePtr a,
        const HittablePtr b,
        int axis
    ) {
        AABB box_a, box_b;
        if (!a->bounding_box(0, 0, box_a) ||
            !b->bounding_box(0, 0, box_b))
        {
            std::cerr << "No bounding box in BVHNode constructor.\n";
        }
        return box_a.min()[axis] < box_b.min()[axis];
    }
}

BVHNode::BVHNode() = default;

BVHNode::BVHNode(
    const std::vector<HittablePtr>& src_objects,
    size_t start,
    size_t end,
    double time0,
    double time1
) {
    // Копируем указатели, чтобы иметь возможность сортировки
    auto objects = src_objects;

    // Выбираем случайную ось 0=X,1=Y,2=Z
    int axis = std::rand() % 3;
    auto comparator = (axis == 0)
        ? [](const HittablePtr&a,const HittablePtr&b){ return box_compare_axis(a,b,0); }
        : (axis == 1)
            ? [](const HittablePtr&a,const HittablePtr&b){ return box_compare_axis(a,b,1); }
            : [](const HittablePtr&a,const HittablePtr&b){ return box_compare_axis(a,b,2); };

    size_t object_span = end - start;

    if (object_span == 1) {
        left  = right = objects[start];
    }
    else if (object_span == 2) {
        if (comparator(objects[start], objects[start+1])) {
            left  = objects[start];
            right = objects[start+1];
        } else {
            left  = objects[start+1];
            right = objects[start];
        }
    }
    else {
        std::sort(objects.begin() + start,
                  objects.begin() + end,
                  comparator);
        size_t mid = start + object_span / 2;
        left  = std::make_shared<BVHNode>(objects, start, mid, time0, time1);
        right = std::make_shared<BVHNode>(objects, mid,   end, time0, time1);
    }

    AABB box_left, box_right;
    if (!left->bounding_box(time0, time1, box_left) ||
        !right->bounding_box(time0, time1, box_right))
    {
        std::cerr << "No bounding box in BVHNode constructor.\n";
    }

    box = AABB::surrounding_box(box_left, box_right);
}

bool BVHNode::hit(
    const Ray& r,
    double t_min,
    double t_max,
    HitRecord& rec
) const {
    if (!box.hit(r, t_min, t_max))
        return false;

    bool hit_left  = left->hit(r, t_min, t_max, rec);
    bool hit_right = right->hit(
        r,
        t_min,
        hit_left ? rec.t : t_max,
        rec
    );
    return hit_left || hit_right;
}

bool BVHNode::bounding_box(
    double time0,
    double time1,
    AABB& output_box
) const {
    output_box = box;
    return true;
}

// Определяем свободные функции-компараторы
bool box_x_compare(const HittablePtr a, const HittablePtr b) {
    return box_compare_axis(a, b, 0);
}
bool box_y_compare(const HittablePtr a, const HittablePtr b) {
    return box_compare_axis(a, b, 1);
}
bool box_z_compare(const HittablePtr a, const HittablePtr b) {
    return box_compare_axis(a, b, 2);
}
