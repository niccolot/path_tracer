#include <iostream>
#include <memory>

#include "camera.h"
#include "sphere.h"
#include "material.h"
#include "bvh.h"

int main() {
    
    int img_width = 512;
    double aspect_ratio = 16.0 / 9.0;

    HittableList world;

    auto Material_ground = std::make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
    auto Material_center = std::make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
    auto Material_left   = std::make_shared<Dielectric>(1.5);
    auto Material_right  = std::make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.3);

    auto center1 = Vec3(0.0, 0.0, -1.2);
    auto center2 = center1 + Vec3(0.0, 0.25, -1.2);

    world.add(std::make_shared<Sphere>(Vec3( 0.0, -100.5, -1.0), 100.0, Material_ground));
    world.add(std::make_shared<Sphere>(center1, center2, 0.5, Material_center));
    world.add(std::make_shared<Sphere>(Vec3(-1.0,    0.0, -1.0),   0.5, Material_left));
    world.add(std::make_shared<Sphere>(Vec3( 1.0,    0.0, -1.0),   0.5, Material_right));

    Camera cam(img_width, aspect_ratio, Vec3(0,0,1), Vec3(0,0,.99), 45);

    world = HittableList(std::make_shared<BVHNode>(world));

    cam.render(world);
    
    
     
    /*
    HittableList world;

    auto ground_Material = std::make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    world.add(std::make_shared<Sphere>(Vec3(0,-1000,0), 1000, ground_Material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            Vec3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - Vec3(4, 0.2, 0)).length() > 0.9) {
                std::shared_ptr<Material> Sphere_Material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = Color::random() * Color::random();
                    Sphere_Material = std::make_shared<Lambertian>(albedo);
                    world.add(std::make_shared<Sphere>(center, 0.2, Sphere_Material));
                } else if (choose_mat < 0.95) {
                    // Metal
                    auto albedo = Color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    Sphere_Material = std::make_shared<Metal>(albedo, fuzz);
                    world.add(std::make_shared<Sphere>(center, 0.2, Sphere_Material));
                } else {
                    // glass
                    Sphere_Material = std::make_shared<Dielectric>(1.5);
                    world.add(std::make_shared<Sphere>(center, 0.2, Sphere_Material));
                }
            }
        }
    }

    auto Material1 = std::make_shared<Dielectric>(1.5);
    world.add(std::make_shared<Sphere>(Vec3(0, 1, 0), 1.0, Material1));

    auto Material2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    world.add(std::make_shared<Sphere>(Vec3(-4, 1, 0), 1.0, Material2));

    auto Material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<Sphere>(Vec3(4, 1, 0), 1.0, Material3));

    Camera cam(512, 16./9., Vec3(3,2,3), Vec3(0,0,0), 90, 10., 0.6, 100);

    //world = HittableList(std::make_shared<BVHNode>(world));

    cam.render(world);
    */
    
    
}