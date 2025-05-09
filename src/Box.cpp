#include "Box.h"
#include <memory>

#include "XYRect.h"
#include "XZRect.h"
#include "YZRect.h"
#include "HittableList.h"

bool Box::hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const {
    HittableList sides;
    sides.add(std::make_shared<XYRect>(
      box_min.x, box_max.x, box_min.y, box_max.y, box_max.z, mat_ptr));
    sides.add(std::make_shared<XYRect>(
      box_min.x, box_max.x, box_min.y, box_max.y, box_min.z, mat_ptr));
    sides.add(std::make_shared<XZRect>(
      box_min.x, box_max.x, box_min.z, box_max.z, box_max.y, mat_ptr));
    sides.add(std::make_shared<XZRect>(
      box_min.x, box_max.x, box_min.z, box_max.z, box_min.y, mat_ptr));
    sides.add(std::make_shared<YZRect>(
      box_min.y, box_max.y, box_min.z, box_max.z, box_max.x, mat_ptr));
    sides.add(std::make_shared<YZRect>(
      box_min.y, box_max.y, box_min.z, box_max.z, box_min.x, mat_ptr));
    return sides.hit(r, t_min, t_max, rec);
}
