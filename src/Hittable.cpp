#include "Hittable.h"
#include <algorithm>  // для std::swap, если потребуется

void HitRecord::set_face_normal(
    const Ray& r,
    const Vec3& outward_normal
) {
    front_face = dot(r.direction, outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
}
