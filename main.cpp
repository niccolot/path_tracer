#include <iostream>
#include <memory>

#include "camera.h"
#include "sphere.h"
#include "material.h"

int main() {

    int img_width = 512;
    double aspect_ratio = 16.0 / 9.0;

    HittableList world;

    auto material_ground = std::make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
    auto material_left   = std::make_shared<Dielectric>(1.5);
    auto material_right  = std::make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.3);

    world.add(std::make_shared<Sphere>(Vec3( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(std::make_shared<Sphere>(Vec3( 0.0,    0.0, -1.2),   0.5, material_center));
    world.add(std::make_shared<Sphere>(Vec3(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(std::make_shared<Sphere>(Vec3( 1.0,    0.0, -1.0),   0.5, material_right));

    Camera cam(img_width, aspect_ratio, Vec3(0,0,1), Vec3(0,0,.99), 45);

    cam.render(world);
}