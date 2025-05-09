#include "Vec3.h"

#include <cmath>
#include <random>
#include <iostream>

namespace {
    static thread_local std::mt19937 generator(std::random_device{}());
    static inline double random_double_unit() {
        static std::uniform_real_distribution<double> dist(0.0, 1.0);
        return dist(generator);
    }
}

Vec3::Vec3() : x(0), y(0), z(0) {}

Vec3::Vec3(double e0, double e1, double e2)
    : x(e0), y(e1), z(e2)
{}

Vec3 Vec3::operator-() const {
    return Vec3(-x, -y, -z);
}

double Vec3::operator[](int i) const {
    return (i == 0 ? x : (i == 1 ? y : z));
}

double& Vec3::operator[](int i) {
    return (i == 0 ? x : (i == 1 ? y : z));
}

Vec3& Vec3::operator+=(const Vec3 &v) {
    x += v.x; y += v.y; z += v.z;
    return *this;
}

Vec3& Vec3::operator*=(double t) {
    x *= t; y *= t; z *= t;
    return *this;
}

Vec3& Vec3::operator/=(double t) {
    return *this *= 1/t;
}

double Vec3::length() const {
    return std::sqrt(length_squared());
}

double Vec3::length_squared() const {
    return x*x + y*y + z*z;
}

Vec3 Vec3::random() {
    return Vec3(
        random_double_unit(),
        random_double_unit(),
        random_double_unit()
    );
}

Vec3 Vec3::random(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return Vec3(dist(generator), dist(generator), dist(generator));
}


std::ostream& operator<<(std::ostream &out, const Vec3 &v) {
    return out << v.x << ' ' << v.y << ' ' << v.z;
}

Vec3 operator+(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}

Vec3 operator-(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.x - v.x, u.y - v.y, u.z - v.z);
}

Vec3 operator*(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.x * v.x, u.y * v.y, u.z * v.z);
}

Vec3 operator*(double t, const Vec3 &v) {
    return Vec3(t * v.x, t * v.y, t * v.z);
}

Vec3 operator*(const Vec3 &v, double t) {
    return t * v;
}

Vec3 operator/(Vec3 v, double t) {
    return (1/t) * v;
}

double dot(const Vec3 &u, const Vec3 &v) {
    return u.x * v.x + u.y * v.y + u.z * v.z;
}

Vec3 cross(const Vec3 &u, const Vec3 &v) {
    return Vec3(
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x
    );
}

Vec3 unit_vector(Vec3 v) {
    return v / v.length();
}

double random_double() {

    return Vec3::random().x;
}

double random_double(double min, double max) {
    return Vec3::random(min, max).x;
}
