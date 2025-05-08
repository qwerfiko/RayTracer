// src/Main.cpp

#include <limits>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <chrono>
#include <iomanip>   // для std::setprecision

#include "Vec3.h"
#include "Ray.h"
#include "HittableList.h"
#include "Sphere.h"
#include "Material.h"
#include "Camera.h"
#include "BVH.h"
#include "XZRect.h"
#include "XYRect.h"
#include "Box.h"
#include "Cone.h"
#include "NoiseTexture.h"
#include "WoodTexture.h"
#include "ConstantTexture.h"


using namespace std;
namespace fs = std::filesystem;

static double ambient_occlusion(const Point3& p, const Vec3& normal, const Hittable& world) {
    const int AO_SAMPLES = 8;          // число проб (можно уменьшить для скорости)
    int   occluded   = 0;
    HitRecord tmp;
    for (int i = 0; i < AO_SAMPLES; ++i) {
        Vec3 dir = random_in_hemisphere(normal);
        // смещаем точку немного по нормали, чтобы не «самопересечься»
        Ray ao_ray(p + 1e-4*normal, dir);
        if (world.hit(ao_ray, 0.001, std::numeric_limits<double>::infinity(), tmp))
            ++occluded;
    }
    // чем больше occluded, тем меньше освещённость
    return 1.0 - double(occluded) / AO_SAMPLES;
}


// трассировка луча
Color ray_color(const Ray& r, const Hittable& world, int depth) {
    if (depth <= 0)
        return Color(0,0,0);

    HitRecord rec;
    if (world.hit(r, 0.001, std::numeric_limits<double>::infinity(), rec)) {
        // 1) эмиссия материала (DiffuseLight)
        Color emitted = rec.mat_ptr->emitted();
        if (emitted.x>0 || emitted.y>0 || emitted.z>0) {
            // эмиттер: AO не применяется
            return emitted;
        }

        // 2) Scatter
        ScatterRecord srec;
        if (!rec.mat_ptr->scatter(r, rec, srec)) {
            // некоторая «black-hole» защита
            return Color(0,0,0);
        }

        // 3) specular — пропускаем без AO
        if (srec.is_specular) {
            return srec.attenuation
                 * ray_color(srec.specular_ray, world, depth-1);
        }

        // 4) lambertian (diffuse) — только здесь считаем AO
        //    и умножаем им только диффузную составляющую
        double ao = ambient_occlusion(rec.p, rec.normal, world);

        Color diffuse = srec.attenuation
                      * ray_color(srec.specular_ray, world, depth-1);

        return emitted + ao * diffuse;
    }

    // 5) фон
    Vec3 u = unit_vector(r.direction);
    double t = 0.5*(u.y + 1.0);
    return (1.0 - t)*Color(1.0,1.0,1.0)
         +         t*Color(0.5,0.7,1.0);
}


int main() {
    // 1) Параметры рендера
    const double aspect_ratio      = 16.0/9.0;
    const int    image_width       = 720;
    const int    image_height      = static_cast<int>(image_width/aspect_ratio);
    const int    samples_per_pixel = 100;
    const int    max_depth         = 25;
    const int    thread_count      = thread::hardware_concurrency();


    // 2) Материалы
    auto mat_ground = make_shared<Lambertian>(
        make_shared<ConstantTexture>(Color(0.8,0.8,0.0))
    );
    // светящаяся плоскость
    auto mat_light = make_shared<DiffuseLight>(Color(6.0,6.0,6.0));

    // диффузные шары
    auto mat_diffuse = make_shared<Lambertian>(
        make_shared<ConstantTexture>(Color(0.1,0.2,0.5))
    );
    // стекло и металл
    auto mat_glass = make_shared<Dielectric>(1.2);
    auto mat_metal = make_shared<Metal>(Color(0.8,0.8,0.8), 0.0);



    // 3) Сцена
    HittableList world;

    // 3.1) Ground: большая XZ-плоскость y = 0
    world.add(make_shared<XZRect>(
        -10, +10,   // x0, x1
        -10, +10,   // z0, z1
         0.0,       // y = 0
         mat_ground
    ));

    // 3.2) Источник света — XY-плоскость позади сцены на z = -5
    world.add(make_shared<XYRect>(
        -10, +10,   // x0, x1
        -10, +10,   // y0, y1
        -5.0,       // z = -5
        mat_light
    ));

    // 3.3) Пирамидка из трёх шаров
    // нижний ряд
    world.add(make_shared<Sphere>(Point3(1.0, 0.5, -1.0), 0.5, mat_diffuse));
    world.add(make_shared<Sphere>(Point3(-1.0, 0.5, -1.5), 0.5, mat_glass));
    world.add(make_shared<Sphere>(Point3(0.0, 0.5, -1.0), 0.5, mat_metal));

    // procedural textures
   auto noise_tex = make_shared<NoiseTexture>(4.0);
   auto mat_noise = make_shared<Lambertian>(noise_tex);

   auto wood_tex  = make_shared<WoodTexture>(10.0);
   auto mat_wood  = make_shared<Lambertian>(wood_tex);

   // куб с «шумовой» текстурой слева
   world.add(make_shared<Box>(
       Point3(-3.0, 0.0, -2.5),
       Point3(-2.0, 1.0, -1.5),
       mat_noise
   ));

   // конус с «деревянной» текстурой справа
   world.add(make_shared<Cone>(
       Point3(2.0, 2.0, -2.0),   // вершина конуса
       Vec3(0.,-1.0, 0.0),       // ось вдоль Y
       M_PI/6,                    // угол 30°
       2.0,                       // высота
       mat_wood
   ));

    // BVH для ускорения
    vector<HittablePtr> objs = world.objects;
    BVHNode bvh(objs, 0, objs.size(), 0.0, 1.0);

    // 4) Камера с DOF
    Point3 lookfrom( 0.0, 2.0,  3.0 );
    Point3 lookat  ( 0.0, 1.0, -1.5 );
    Vec3   vup     ( 0.0, 1.0,  0.0 );
    double dist_to_focus = (lookfrom - lookat).length();
    double aperture     = 0.15;    // >0 — будет размытие вне зоны фокуса
    Camera cam(
        lookfrom,
        lookat,
        vup,
        40.0,           // vfov
        aspect_ratio,
        aperture,
        dist_to_focus
    );

    // 5) Рендер

    std::vector<Color> framebuffer(image_width * image_height);
    std::atomic<int>   lines_done{0};
    std::atomic<bool>  render_done{false};
    auto               start_time = std::chrono::steady_clock::now();

    // --- Запуск рендер-потоков ---
    std::vector<std::thread> threads;
    for (int t = 0; t < thread_count; ++t) {
        threads.emplace_back([&, t]() {
            for (int j = image_height - 1 - t; j >= 0; j -= thread_count) {
                for (int i = 0; i < image_width; ++i) {
                    Color col(0,0,0);
                    for (int s = 0; s < samples_per_pixel; ++s) {
                        double u = (i + random_double()) / (image_width  - 1);
                        double v = (j + random_double()) / (image_height - 1);
                        Ray    r = cam.get_ray(u, v);
                        col += ray_color(r, bvh, max_depth);
                    }
                    // среднее + гамма
                    col /= samples_per_pixel;
                    col.x = std::sqrt(col.x);
                    col.y = std::sqrt(col.y);
                    col.z = std::sqrt(col.z);

                    framebuffer[j * image_width + i] = col;
                }
                ++lines_done;  // одна строка закончена
            }
        });
    }

    // --- Поток-монитор прогресса ---
    std::thread progress_thread([&]() {
        using namespace std::chrono;
        while (!render_done.load()) {
            int done = lines_done.load();
            double frac = double(done) / image_height;
            auto   now  = steady_clock::now();
            double elapsed   = duration<double>(now - start_time).count();
            double total_est = frac > 0 ? (elapsed / frac) : 0.0;
            double remaining = total_est - elapsed;

            std::cout << "\rRendering: "
                      << std::setfill(' ') << std::setw(3) << int(frac*100) << "%  "
                      << " elapsed: "  << std::fixed << std::setprecision(1) << elapsed << "s  "
                      << " remaining: "<< std::setprecision(1) << remaining << "s   "
                      << std::flush;
            std::this_thread::sleep_for(seconds(1));
        }
        // конечный отчёт
        auto total = duration<double>(steady_clock::now() - start_time).count();
        std::cout << "\rRendering: 100%  total time: "
                  << std::fixed << std::setprecision(1) << total << "s          \n";
    });

    // --- Ждём завершения рендер-потоков ---
    for (auto &th : threads) th.join();
    render_done = true;
    progress_thread.join();

    // --- Вывод готового изображения в PPM ---
    std::ofstream out("output/image.ppm");
    out << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for (int j = image_height - 1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            Color c = framebuffer[j * image_width + i];
            int ir = static_cast<int>(256 * std::clamp(c.x, 0.0, 0.999));
            int ig = static_cast<int>(256 * std::clamp(c.y, 0.0, 0.999));
            int ib = static_cast<int>(256 * std::clamp(c.z, 0.0, 0.999));
            out << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }

    std::cout << "Render complete.\n";
    return 0;
}
