#include <iostream>
#include <memory>

#include "camera.h"
#include "sphere.h"
#include "material.h"
#include "bvh.h"
#include "texture.h"
#include "quad.h"

void test() {
    int img_width = 512;
    double aspect_ratio = 16.0 / 9.0;

    HittableList world;

    auto noise_texture = std::make_shared<NoiseTexture>(4);

    auto Material_ground = std::make_shared<Lambertian>(noise_texture);
    auto Material_center = std::make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
    auto Material_left   = std::make_shared<Dielectric>(1.5);
    auto Material_right  = std::make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.3);

    auto center1 = Vec3(0.0, 0.0, -1.2);

    world.add(std::make_shared<Sphere>(Vec3( 0.0, -100.5, -1.0), 100.0, Material_ground));
    world.add(std::make_shared<Sphere>(center1, 0.5, Material_center));
    world.add(std::make_shared<Sphere>(Vec3(-1.0,    0.0, -1.0),   0.5, Material_left));
    world.add(std::make_shared<Sphere>(Vec3( 1.0,    0.0, -1.0),   0.5, Material_right));

    Camera cam(img_width, aspect_ratio, Vec3(0,0,1), Vec3(0,0,.99), 90);

    cam.render(world);
}

void random_spheres() {
     HittableList world;

    auto checker = std::make_shared<CheckerTexture>(0.32, Color(.2, .3, .1), Color(.9, .9, .9));
    world.add(std::make_shared<Sphere>(Vec3(0,-1000,0), 1000, std::make_shared<Lambertian>(checker)));

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

    world = HittableList(std::make_shared<BVHNode>(world));

    cam.render(world);
}

void quads() {

    using std::make_shared;
    using std::shared_ptr;

    HittableList world;

    // Materials
    auto left_red     = make_shared<Lambertian>(Color(1.0, 0.2, 0.2));
    auto back_green   = make_shared<Lambertian>(Color(0.2, 1.0, 0.2));
    auto right_blue   = make_shared<Lambertian>(Color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<Lambertian>(Color(1.0, 0.5, 0.0));
    auto lower_teal   = make_shared<Lambertian>(Color(0.2, 0.8, 0.8));

    // Quads
    world.add(make_shared<Quad>(Vec3(-3,-2, 5), Vec3(0, 0,-4), Vec3(0, 4, 0), left_red));
    world.add(make_shared<Quad>(Vec3(-2,-2, 0), Vec3(4, 0, 0), Vec3(0, 4, 0), back_green));
    world.add(make_shared<Quad>(Vec3( 3,-2, 1), Vec3(0, 0, 4), Vec3(0, 4, 0), right_blue));
    world.add(make_shared<Quad>(Vec3(-2, 3, 1), Vec3(4, 0, 0), Vec3(0, 0, 4), upper_orange));
    world.add(make_shared<Quad>(Vec3(-2,-3, 5), Vec3(4, 0, 0), Vec3(0, 0,-4), lower_teal));

    Camera cam(400, 1., Vec3(0,0,3), Vec3(0,0,0), 80);

    cam.render(world);
}

void debug() {
    HittableList world;
    auto red = std::make_shared<Lambertian>(Color(1,0,0));
    //world.add(std::make_shared<Sphere>(Vec3(0,0,-1), 0.5, red));
    world.add(std::make_shared<Quad>(Vec3(0,0,-1), Vec3(1,0,0), Vec3(0,1,0), red));
    Camera cam(400, 16./9., Vec3(0.5,0.5,0.5), Vec3(0.5,0.5,-1), 80., 1.);
    cam.render(world);
}

int main() {
    
    debug();
}