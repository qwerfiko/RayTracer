#include "Cone.h"
#include <limits>

bool Cone::hit(
    const Ray& r,
    double t_min,
    double t_max,
    HitRecord& rec
) const {
    // переводим в локальную систему с началом в apex
    Vec3 O = r.origin - apex;
    Vec3 D = r.direction;

    double k   = std::tan(half_angle);
    double k2  = k*k;

    // проекции на ось
    double dV = dot(D, axis);
    double oV = dot(O, axis);

    // компоненты перпендикулярные оси
    Vec3 Dp = D - dV*axis;
    Vec3 Op = O - oV*axis;

    double A = dot(Dp, Dp) - k2 * dV*dV;
    double B = 2*( dot(Dp, Op) - k2 * dV*oV );
    double C = dot(Op, Op) - k2 * oV*oV;

    double disc = B*B - 4*A*C;
    if (disc < 0) return false;
    double sqrt_disc = std::sqrt(disc);

    auto check_root = [&](double t) {
        if (t < t_min || t > t_max) return false;
        // узнать, попадает ли точка на конечный отрезок конуса
        double y = oV + t*dV;
        if (y < 0 || y > height) return false;

        // заполняем HitRecord
        rec.t = t;
        rec.p = r.at(t);

        // нормаль конуса: вектор из точки к боковой поверхности
        // сначала вычисляем высотную проекцию
        Vec3 proj = axis * dot(rec.p - apex, axis);
        // радиусная составляющая
        Vec3 rad  = (rec.p - apex) - proj;
        // нормаль — направление комбинированного градиента
        Vec3 n = unit_vector(rad - k2 * proj);
        rec.set_face_normal(r, n);
        rec.mat_ptr = mat_ptr;
        return true;
    };

    // проверяем два корня
    double t0 = (-B - sqrt_disc) / (2*A);
    if (check_root(t0)) return true;
    double t1 = (-B + sqrt_disc) / (2*A);
    if (check_root(t1)) return true;

    return false;
}

bool Cone::bounding_box(
    double /*time0*/,
    double /*time1*/,
    AABB& output_box
) const {
    // простая AABB, которая покрывает вершину и основание
    Point3 base_center = apex + axis * height;
    double r = height * std::tan(half_angle);

    // для каждой координаты берем min/max из {apex, base_center} и отнимаем/прибавляем r
    double min_x = std::fmin(apex.x, base_center.x) - r;
    double min_y = std::fmin(apex.y, base_center.y) - r;
    double min_z = std::fmin(apex.z, base_center.z) - r;
    double max_x = std::fmax(apex.x, base_center.x) + r;
    double max_y = std::fmax(apex.y, base_center.y) + r;
    double max_z = std::fmax(apex.z, base_center.z) + r;

    output_box = AABB(Point3(min_x, min_y, min_z),
                      Point3(max_x, max_y, max_z));
    return true;
}
