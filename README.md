# Modern C++ Ray Tracer

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue) ![Threads](https://img.shields.io/badge/Multithreaded-Yes-green) 

![image (3)](https://github.com/user-attachments/assets/60fa96be-7c4a-48ee-89f9-e60c2e0b1c16)


## Описание

Этот проект — учебный трассер лучей на **C++17**, демонстрирующий основные приёмы построения физически корректных изображений:
- **Векторная арифметика** (класс `Vec3`)
- **Лучи и пересечения** (`Ray`, `Hittable`, `Sphere`, `BVH`) с поддержкой отражений, преломлений и эмиссии.
- **Материалы**: диффузный, металлический, диэлектрик (стекло), эмиттер (`DiffuseLight`) и процедурные текстуры (дерево)
- **Антиалиасинг** (jitter-сэмплинг), **гамма-коррекция**, **многопоточность**
- **Буферизованный вывод** для корректного parallel‐рендеринга
- **Ambient Occlusion** (простейший вариант shadows+AO).
## Особенности

- **Реалистичный металл** и **стекло** с отражениями и преломлениями  
- **Светящиеся объекты** (эмиттеры) с собственным излучением  
- **BVH-ускоритель** для быстрого поиска пересечений при большом количестве объектов  
- **Параметризация сцены** — легко менять позиции, материалы и добавлять новые геометрические примитивы  
- **OpenMP-style** многопоточность через `std::thread` и буферизацию вывода

<details>
  <summary>📁 Структура проекта</summary>

## Структура проекта


```text
RayTracerProject/
├── include/
│ ├── Vec3.h
│ ├── Ray.h
│ ├── Hittable.h
│ ├── HittableList.h
│ ├── Sphere.h
│ ├── Box.h
│ ├── Cone.h
│ ├── AABB.h
│ ├── BVH.h
│ ├── Camera.h
│ ├── Material.h
│ ├── Texture.h
│ ├── ConstantTexture.h
│ ├── NoiseTexture.h
│ └── WoodTexture.h
└── src/
├── Vec3.cpp
├── Ray.cpp
├── Hittable.cpp
├── Sphere.cpp
├── Box.cpp
├── Cone.cpp
├── BVH.cpp
├── Camera.cpp
├── Material.cpp
├── Texture.cpp
├── Perlin.cpp
└── Main.cpp
```

## Алгоритм трассировки 🛠
Для каждого пикселя генерируется N случайных лучей (anti-aliasing).

**ray_color()** рекурсивно:

- Ищет ближайший hit в диапазоне [ε, +∞).

- Вызывает material.scatter(), получая цвет отражённой/преломлённой составляющей и attenuation.

- Добавляет emission для источников (DiffuseLight).

- Для теней/AO дополнительно бросает shadow‐ray и затемняет вклады.

**Ускорение**: при большом числе объектов — BVH ускоряет поиск пересечений.

**Параллелизация**: каждый поток обрабатывает строки изображения независимо.



## Сборка и запуск 🚀

1. Клонируй репозиторий и перейди в папку проекта:
    ```bash
    git clone git@github.com:qwerfiko/RayTracer.git
    ```
2. Убедись, что установлен **g++**, поддерживающий C++17, и подключен OpenMP-стандарт:
    ```bash
    g++ --version   # минимум GCC 7+
    ```
3. Собери проект и запусти трассер:
    ```bash
    g++ -std=c++17 -O2 -I include src/*.cpp -o raytracer -pthread
    ./raytracer
    ```
4. Открой файл любым просмотрщиком ppm, например:
    ```bash
    display output/image.ppm
    # или любым другим просмотрщиком PPM
    ```
5. Можно использовать **CMake** со следующим содержанием:
   ```bash
    cmake_minimum_required(VERSION 3.10)
    project(RayTracer LANGUAGES CXX)
    set(CMAKE_CXX_STANDARD 17)
    file(GLOB SOURCES src/*.cpp)
    add_executable(raytracer ${SOURCES})
    target_include_directories(raytracer PRIVATE include)
   ```

## Настройка сцены ⚙️

- **Разрешение** меняется в `Main.cpp` (в примере используется FullHD с соотношением сторон 16:9):
  ```cpp
  const int image_width  = 1920;
  const int image_height = static_cast<int>(image_width / aspect_ratio);
  ```
- Параметры рендера: *samples_per_pixel* = 500, *max_depth* = 50, *AO_samples* = 32
- С помощью *world.add* добавляются объекты в сцену с соответсвующим параметром *mat_*
- Выставляется положение камеры, focus и aperture
- Рендер в в формате ppm сохраняет построчно в framebufer и осуществляет gamma-коррекцию
