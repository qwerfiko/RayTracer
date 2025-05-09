// Ray: луч с началом origin и направлением direction.
// Метод at(t) возвращает точку вдоль луча.
#pragma once

#include "Vec3.h"

/**
 * @brief Класс луча: точка начала и направление.
 */
class Ray {
public:
    Point3 origin;
    Vec3   direction;

    Ray();
    Ray(const Point3& origin, const Vec3& direction);

    /**
     * @brief Возвращает точку на луче с параметром t.
     */
    Point3 at(double t) const;
};
