#pragma once
#include <cmath>
#include <iostream>

class Vec3 {
public:
    double x, y, z;

    Vec3();
    Vec3(double e0, double e1, double e2);

    Vec3 operator-() const;
    Vec3& operator+=(const Vec3& v);
    Vec3& operator*=(double t);
    Vec3& operator/=(double t);

    double length() const;
    double length_squared() const;

    static Vec3 random();
    static Vec3 random(double min, double max);

    // Добавляем доступ по индексу
    double operator[](int i) const;
    double& operator[](int i);
};

// Псевдонимы
using Point3 = Vec3;
using Color  = Vec3;

// Свободные функции
std::ostream& operator<<(std::ostream &out, const Vec3 &v);
Vec3 operator+(const Vec3 &u, const Vec3 &v);
Vec3 operator-(const Vec3 &u, const Vec3 &v);
Vec3 operator*(const Vec3 &u, const Vec3 &v);
Vec3 operator*(double t, const Vec3 &v);
Vec3 operator*(const Vec3 &v, double t);
Vec3 operator/(Vec3 v, double t);
double dot(const Vec3 &u, const Vec3 &v);
Vec3 cross(const Vec3 &u, const Vec3 &v);
Vec3 unit_vector(Vec3 v);

// Глобальный генератор одного случайного double в [0,1)
double random_double();

// Случайная точка в полусфёре вокруг данной нормали
inline Vec3 random_in_hemisphere(const Vec3& normal) {
    while (true) {
        auto p = Vec3::random(-1,1);
        if (p.length_squared() >= 1) continue;
        if (dot(p, normal) > 0.0) // в том же полушарии, что normal
            return unit_vector(p);
        else
            return unit_vector(-p);
    }
}
