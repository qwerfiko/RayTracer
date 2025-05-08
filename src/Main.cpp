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

// трассировка луча
Color ray_color(const Ray& r, const Hittable& world, int depth) {
    if (depth <= 0) return Color(0,0,0);

    HitRecord rec;
    if (world.hit(r, 0.001, numeric_limits<double>::infinity(), rec)) {
        if (world.hit(r, 0.001, numeric_limits<double>::infinity(), rec)) {
        Color emitted = rec.mat_ptr->emitted();
        ScatterRecord srec;
        if (rec.mat_ptr->scatter(r, rec, srec)) {
            return emitted
                 + srec.attenuation
                   * ray_color(srec.specular_ray, world, depth-1);
        }
        return emitted;
    }
    }
    // фон: градиент неба
    Vec3 u = unit_vector(r.direction);
    double t = 0.5*(u.y + 1.0);
    return (1.0 - t)*Color(1.0,1.0,1.0)
         +        t*Color(0.5,0.7,1.0);
}

int main() {
    // 1) Параметры рендера
    const double aspect_ratio      = 16.0/9.0;
    const int    image_width       = 720;
    const int    image_height      = static_cast<int>(image_width/aspect_ratio);
    const int    samples_per_pixel = 50;
    const int    max_depth         = 25;
    const int    thread_count      = thread::hardware_concurrency();

    // 2) Материалы
    auto mat_ground = make_shared<Lambertian>(
        make_shared<ConstantTexture>(Color(0.8,0.8,0.0))
    );
    // светящаяся плоскость
    auto mat_light = make_shared<DiffuseLight>(Color(4.0,4.0,4.0));

    // диффузные шары
    auto mat_diffuse = make_shared<Lambertian>(
        make_shared<ConstantTexture>(Color(0.1,0.2,0.5))
    );
    // стекло и металл
    auto mat_glass = make_shared<Dielectric>(1.5);
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

    // 3.3) Пирамидка из четырёх шаров
    // нижний ряд
    world.add(make_shared<Sphere>(Point3(1.0, 0.5, -1.0), 0.5, mat_diffuse));
    world.add(make_shared<Sphere>(Point3(0.0, 0.5, -2.0), 0.5, mat_glass));
    world.add(make_shared<Sphere>(Point3(0.0, 1.5, -1.5), 0.5, mat_metal));

    // procedural textures
   auto noise_tex = make_shared<NoiseTexture>(4.0);
   auto mat_noise = make_shared<Lambertian>(noise_tex);

   auto wood_tex  = make_shared<WoodTexture>(10.0);
   auto mat_wood  = make_shared<Lambertian>(wood_tex);

   // куб с «шумовой» текстурой слева
   world.add(make_shared<Box>(
       Point3(-2.0, 0.0, -2.5),
       Point3(-1.0, 1.0, -1.5),
       mat_noise
   ));

   // конус с «деревянной» текстурой справа
   world.add(make_shared<Cone>(
       Point3( 2.0, 2.0, -2.0),   // вершина конуса
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
    fs::create_directory("output");
    ofstream out("output/image.ppm");
    out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    vector<Color> framebuffer(image_width * image_height);

    // многопоточное заполнение буфера
    vector<thread> threads;
    for (int t = 0; t < thread_count; ++t) {
        threads.emplace_back([&,t](){
            for (int j = image_height-1-t; j >= 0; j -= thread_count) {
                for (int i = 0; i < image_width; ++i) {
                    Color col(0,0,0);
                    for (int s = 0; s < samples_per_pixel; ++s) {
                        double u = (i + random_double())/(image_width-1);
                        double v = (j + random_double())/(image_height-1);
                        Ray r = cam.get_ray(u,v);
                        col += ray_color(r, bvh, max_depth);
                    }
                    // 1) усредняем
col /= samples_per_pixel;
col.x = std::sqrt(col.x);
col.y = std::sqrt(col.y);
col.z = std::sqrt(col.z);

                    framebuffer[j*image_width + i] = col;
                }
            }
        });
    }
    for (auto &th : threads) th.join();

    // выводим буфер в PPM
    for (int j = image_height-1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            Color c = framebuffer[j*image_width + i];
            int ir = static_cast<int>(256 * std::clamp(c.x, 0.0, 0.999));
            int ig = static_cast<int>(256 * std::clamp(c.y, 0.0, 0.999));
            int ib = static_cast<int>(256 * std::clamp(c.z, 0.0, 0.999));
            out << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }

    cout << "Render complete.\n";
    return 0;
}
