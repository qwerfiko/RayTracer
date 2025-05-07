// src/Main.cpp

#include <limits>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>   // std::clamp
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>

#include "Vec3.h"
#include "Ray.h"
#include "HittableList.h"
#include "Sphere.h"
#include "Material.h"
#include "Camera.h"
#include "BVH.h"


using namespace std;
namespace fs = std::filesystem;

// Рекурсивный трассер: отражения/преломления через scatter()
Color ray_color(const Ray& r, const Hittable& world, int depth) {
    if (depth <= 0)
        return Color(0,0,0);

    HitRecord rec;
    // вместо inf используем numeric_limits<>::infinity()
    if (world.hit(r, 0.001, std::numeric_limits<double>::infinity(), rec)) {
        // собственное излучение материала
        Color emitted = rec.mat_ptr->emitted();

        ScatterRecord srec;
        // если это не источник света – scatter вернёт true
        if (rec.mat_ptr->scatter(r, rec, srec)) {
            return emitted
                 + srec.attenuation
                   * ray_color(srec.specular_ray, world, depth - 1);
        }
        // источник света – только emitted
        return emitted;
    }

    // фон – градиент неба
    Vec3 unit_dir = unit_vector(r.direction);
    auto t = 0.5 * (unit_dir.y + 1.0);
    return (1.0 - t) * Color(1.0,1.0,1.0)
         +        t * Color(0.5,0.7,1.0);
}

int main() {
    // 1) Параметры
    const double aspect_ratio      = 16.0/9.0;
    const int    image_width       = 1920;
    const int    image_height      = static_cast<int>(image_width / aspect_ratio);
    const int    samples_per_pixel = 50;
    const int    max_depth         = 25;
    const int thread_count = thread::hardware_concurrency();

    mutex mtx;
atomic<int> lines_done{0};

// ← вот здесь
vector<thread> threads;

    // 2) Сцена
    HittableList world;
    auto mat_ground  = make_shared<Lambertian>(Color(0.8,0.8,0.0));
    auto mat_diffuse = make_shared<Lambertian>(Color(0.1,0.2,0.5));
    auto mat_glass   = make_shared<Dielectric>(1.05);
    auto mat_metal   = make_shared<Metal>(Color(0.8,0.8,0.8), 0.0);

    // “Плоскость” ground
    world.add(make_shared<Sphere>(
        Point3( 0.0, -100.5, -1.0), 100.0, mat_ground));
    // Диффузный шар
    world.add(make_shared<Sphere>(
        Point3( 0.0,   0.5, -1.0),   0.5, mat_diffuse));
    // Стеклянный шар слева
    world.add(make_shared<Sphere>(
        Point3(-1.0,   0.5, -1.0),   0.5, mat_glass));
    // Металлический шар справа
    world.add(make_shared<Sphere>(
        Point3( 1.0,   0.5, -1.0),   0.5, mat_metal));
    // источник света: белое, сильное излучение
    auto mat_light = make_shared<DiffuseLight>(Color(4.0, 4.0, 4.0));
    // ставим его чуть выше стеклянного шара
    world.add(make_shared<Sphere>(Point3(-1.0, 1.5, -1.0), 0.3, mat_light));

    vector<HittablePtr> objs = world.objects;
    BVHNode bvh(objs, 0, objs.size(), 0.0, 1.0);

    // 3) Камера
    Point3 lookfrom(0.0, 1.5, 3.0);
    Point3 lookat  (0.0, 0.0, -1.0);
    Vec3   vup     (0.0, 1.0,  0.0);
    double vfov    = 40.0;
    double aperture= 0.0;
    double focus_d = (lookfrom - lookat).length();
    Camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, focus_d);

   // Открываем файл
    fs::create_directory("output");
    ofstream out("output/image.ppm");
    out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

// 2) Буфер
int total_pixels = image_width * image_height;
vector<Color> framebuffer(total_pixels);

// 3) Запускаем рендер-потоки
for (int t = 0; t < thread_count; ++t) {
    threads.emplace_back([&, t]() {
        for (int j = image_height - 1 - t; j >= 0; j -= thread_count) {
            for (int i = 0; i < image_width; ++i) {
                Color pixel_color(0,0,0);
                for (int s = 0; s < samples_per_pixel; ++s) {
                    double u = (i + random_double()) / (image_width  - 1);
                    double v = (j + random_double()) / (image_height - 1);
                    pixel_color += ray_color(cam.get_ray(u,v), bvh, max_depth);

                }
                pixel_color /= double(samples_per_pixel);
                // гамма
                pixel_color.x = std::sqrt(pixel_color.x);
                pixel_color.y = std::sqrt(pixel_color.y);
                pixel_color.z = std::sqrt(pixel_color.z);

                int idx = j * image_width + i;
                framebuffer[idx] = pixel_color;
            }
        }
    });
}
for (auto &th : threads) th.join();

// 4) Выводим буфер в файл в правильном порядке
for (int j = image_height - 1; j >= 0; --j) {
    for (int i = 0; i < image_width; ++i) {
        Color c = framebuffer[j * image_width + i];
        int ir = static_cast<int>(256 * std::clamp(c.x, 0.0, 0.999));
        int ig = static_cast<int>(256 * std::clamp(c.y, 0.0, 0.999));
        int ib = static_cast<int>(256 * std::clamp(c.z, 0.0, 0.999));
        out << ir << ' ' << ig << ' ' << ib << '\n';
    }
}
}
