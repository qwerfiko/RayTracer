#include "Cone.h"
#include <limits>

// Пересечение луча с усечённым конусом (основание в apex+axis*height)
bool Cone::hit(
    const Ray& r,
    double t_min,
    double t_max,
    HitRecord& rec
) const {
    // Считаем вектор от apex до точки происхождения луча
    Vec3 oc = r.origin - apex;

    // k = tan(half_angle) = radius/height
    double k    = radius / height;
    double k2   = k * k;

    // Проекция направления и oc на ось конуса
    double dv = dot(r.direction, axis);
    double ov = dot(oc,          axis);

    // «Поперечные» компоненты
    Vec3 d_perp = r.direction - dv * axis;
    Vec3 o_perp = oc          - ov * axis;

    // Квадратичные коэффициенты
    double A = dot(d_perp, d_perp) - k2 * dv * dv;
    double B = 2 * ( dot(d_perp, o_perp) - k2 * dv * ov );
    double C = dot(o_perp, o_perp) - k2 * ov * ov;

    double disc = B*B - 4*A*C;
    if (disc < 0) return false;
    double sqrtd = std::sqrt(disc);

    // Ищём корни
    double t1 = (-B - sqrtd) / (2*A);
    double t2 = (-B + sqrtd) / (2*A);

    // Выбираем ближайший валидный
    double t_hit = t1;
    if (t_hit < t_min || t_hit > t_max) {
        t_hit = t2;
        if (t_hit < t_min || t_hit > t_max)
            return false;
    }

    // Проверяем, что попали в отрезок от вершины до основания
    Point3 P = r.at(t_hit);
    double v = dot(P - apex, axis);
    if (v < 0 || v > height) return false;

    // Заполняем HitRecord
    rec.t       = t_hit;
    rec.p       = P;
    // нормаль: разность между радиальной составляющей и осевой
    Vec3 proj   = axis * v;
    Vec3 perp   = (P - apex) - proj;
    Vec3 N      = perp * height - axis * radius;
    rec.set_face_normal(r, unit_vector(N));
    rec.mat_ptr = mat_ptr;

    return true;
}

// Axis-aligned bounding box для конуса
bool Cone::bounding_box(
    double /*time0*/,
    double /*time1*/,
    AABB& output_box
) const {
    // Охватываем вершину и центр основания
    Point3 base = apex + axis * height;
    Vec3   rvec(radius, radius, radius);

    AABB b1(apex - rvec, apex + rvec);
    AABB b2(base - rvec, base + rvec);
    output_box = AABB::surrounding_box(b1, b2);
    return true;
}
