#include <iostream>
#include <memory>

#include "camera.h"
#include "sphere.h"
#include "material.h"
#include "bvh.h"
#include "texture.h"
#include "quad.h"
#include "triangle.h"
#include "constant_medium.h"

void test()
{
    HittableList world;
    auto ground = std::make_shared<Lambertian>(Color(0.8, 0.5, 0.0), 1);
    //auto ground = std::make_shared<Dielectric>(1.5, Color(0.8, 0.5, 0.0));
    auto center = std::make_shared<Dielectric>(1.5);
    auto light = std::make_shared<DiffuseLight>(Color(20, 20, 20));

    world.add(std::make_shared<Sphere>(Vec3(0, 0, 0), 0.5, center));
    world.add(std::make_shared<Sphere>(Vec3(0, -100.5, 3), 100, ground));
    world.add(std::make_shared<Quad>(Vec3(0.5, 5, 0.5), Vec3(-0.5, 0, 0), Vec3(0, 0, -0.5), light));

    HittableList lights;
    auto empty = std::shared_ptr<Material>();
    lights.add(std::make_shared<Quad>(Vec3(0.5, 5, 0.5), Vec3(-0.5, 0, 0), Vec3(0, 0, -0.5), empty));
    lights.add(std::make_shared<Sphere>(Vec3(0, 0, 0), 0.5, empty));
    lights.add(std::make_shared<Sphere>(Vec3(0, -100.5, -1), 100, empty));

    Camera cam(400, 16. / 9., Vec3(0, 0, 1), Vec3(0, 0, 0), 90, 10, 0, 50);
    cam.render(world, lights);
}

void triangle()
{
    HittableList world;
    auto triangle = std::make_shared<Lambertian>(Color(0.1, 0.3, 0.8));
    world.add(std::make_shared<Triangle>(Vec3(0, 0, 0), Vec3(1, 0, 0), Vec3(0, -1, 0), triangle));
    auto light = std::make_shared<DiffuseLight>(Color(20, 20, 20));
    world.add(std::make_shared<Quad>(Vec3(0.5, 20, 0.5), Vec3(-0.5, 0, 0), Vec3(0, 0, -0.5), light));
    HittableList lights;
    auto empty = std::shared_ptr<Material>();
    lights.add(std::make_shared<Quad>(Vec3(0.5, 20, 0.5), Vec3(-0.5, 0, 0), Vec3(0, 0, -0.5), empty));

    Camera cam(400, 16. / 9., Vec3(0, 0, 2), Vec3(0, 0, 0));

    cam.render(world, lights);
}

void piramid()
{
    HittableList world;
    auto mat = std::make_shared<Lambertian>(Color(0.1, 0.3, 0.8));
    Quad base(Vec3(0, 0, 0), Vec3(1, 0, 0), Vec3(0, 0, -1), mat);
    auto piramid = quad_pyramid(base, 1., mat);
    world.add(piramid);
    auto light = std::make_shared<DiffuseLight>(Color(20, 20, 20));
    world.add(std::make_shared<Quad>(Vec3(0.5, 20, 0.5), Vec3(-0.5, 0, 0), Vec3(0, 0, -0.5), light));
    HittableList lights;
    auto empty = std::shared_ptr<Material>();
    lights.add(std::make_shared<Quad>(Vec3(0.5, 20, 0.5), Vec3(-0.5, 0, 0), Vec3(0, 0, -0.5), empty));

    Camera cam(400, 16. / 9., Vec3(0, 0, -3), Vec3(0, 0, 0));

    cam.render(world, lights);
}

void testpir()
{
    // Image
    const int image_width = 400;
    const double aspect_ratio = 16.0 / 9.0;
    const int samples_per_pixel = 500;
    const int max_depth = 50;

    // World setup
    HittableList world;

    // Materials
    auto ground_material = std::make_shared<Lambertian>(Color(0.8, 0.8, 0.0));  // Yellow-ish ground
    auto pyramid_material = std::make_shared<Lambertian>(Color(0.7, 0.3, 0.3)); // Red-ish pyramid
    // auto pyramid_material = std::make_shared<Dielectric>(1.5);

    // Pyramid base setup
    Vec3 pyramid_base_Q(0, 0, 0); // Bottom-left corner of the pyramid base
    Vec3 pyramid_base_v(1, 0, 0); // Width of the pyramid base
    Vec3 pyramid_base_u(0, 0, 1); // Depth of the pyramid base
    double pyramid_height = 1.5;

    // Pyramid creation
    Quad pyramid_base_q(pyramid_base_Q, pyramid_base_u, pyramid_base_v, pyramid_material);
    Triangle pyramid_base_t(pyramid_base_Q, pyramid_base_u, pyramid_base_v, pyramid_material);
    auto pyramid_t = triangle_pyramid(pyramid_base_t, pyramid_height, pyramid_material);
    auto pyramid_q = quad_pyramid(pyramid_base_q, pyramid_height, pyramid_material);

    //world.add(pyramid_q);
    auto center = std::make_shared<Lambertian>(Color(0.8, 0.1, 0.1));
    world.add(std::make_shared<Sphere>(Vec3(0, 1, 0), 0.5, center));
    world.add(std::make_shared<Sphere>(Vec3(0, -100, 3), 100, ground_material));
   
    HittableList lights;
    auto light = std::make_shared<DiffuseLight>(Color(20, 20, 20));
    world.add(std::make_shared<Quad>(Vec3(0.5, 5, 0.5), Vec3(-0.5, 0, 0), Vec3(0, 0, -0.5), light));
    auto empty = std::shared_ptr<Material>();
    lights.add(std::make_shared<Quad>(Vec3(0.5, 5, 0.5), Vec3(-0.5, 0, 0), Vec3(0, 0, -0.5), empty));
    lights.add(std::make_shared<Sphere>(Vec3(0, 1, 0), 0.5, empty));

    // Camera setup
    Vec3 lookfrom(5.5, 1, -2.5); // Camera position
    Vec3 lookat(0.5, 0, 0);      // Point to look at
    double vfov = 60.0;          // Vertical field of view in degrees
    Camera cam(
        image_width,
        aspect_ratio,
        std::move(lookfrom),
        std::move(lookat),
        vfov,
        10.0, // focus distance
        0.0,  // defocus angle for depth of field
        samples_per_pixel,
        max_depth);

    // Set background color
    cam.set_background(Color(0.5, 0.7, 1.0)); // Light blue background

    // Render the scene
    cam.render(world, lights);
}

void cornell_box()
{
    HittableList world;

    auto red = std::make_shared<Lambertian>(Color(.65, .05, .05), 1);
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73), 1);
    auto green = std::make_shared<Lambertian>(Color(.12, .45, .15), 1);
    auto light = std::make_shared<DiffuseLight>(Color(15, 15, 15));

    world.add(std::make_shared<Quad>(Vec3(343, 554, 332), Vec3(-130, 0, 0), Vec3(0, 0, -105), light));

    world.add(std::make_shared<Quad>(Vec3(555, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), green));
    world.add(std::make_shared<Quad>(Vec3(0, 0, 0), Vec3(0, 555, 0), Vec3(0, 0, 555), red));
    world.add(std::make_shared<Quad>(Vec3(0, 0, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
    world.add(std::make_shared<Quad>(Vec3(555, 555, 555), Vec3(-555, 0, 0), Vec3(0, 0, -555), white));
    world.add(std::make_shared<Quad>(Vec3(0, 0, 555), Vec3(555, 0, 0), Vec3(0, 555, 0), white));

    std::shared_ptr<Metal> alu = std::make_shared<Metal>(Color(0.8, 0.85, 0.88));
    std::shared_ptr<Hittable> box1 = box(Vec3(0, 0, 0), Vec3(165, 330, 165), alu);
    std::shared_ptr<Hittable> box2 = box(Vec3(0, 0, 0), Vec3(165, 165, 165), white);

    box1 = std::make_shared<RotateY>(box1, 15);
    box1 = std::make_shared<Translate>(box1, Vec3(265, 0, 265));
    box2 = std::make_shared<RotateY>(box2, -18);
    box2 = std::make_shared<Translate>(box2, Vec3(130,0,65));

    world.add(box1);
    //world.add(box2);

    auto phong = std::make_shared<Phong>(Color(0.8, 0.3, 0.4), 0.5, 0.5, 5);
    auto lamb = std::make_shared<Lambertian>(Color(0.8, 0.3, 0.4));
    auto glass = std::make_shared<Dielectric>(1.5);

    world.add(std::make_shared<Sphere>(Vec3(190, 90, 190), 90, lamb));
    //world.add(std::make_shared<Sphere>(Vec3(190, 90, 190), 90, glass));
    //world.add(std::make_shared<Sphere>(Vec3(190, 90, 190), 90, white));

    //world = HittableList(std::make_shared<BVHNode>(world));

    auto empty_mat = std::shared_ptr<Material>();
    HittableList lights;
    lights.add(std::make_shared<Quad>(Vec3(213, 554, 227), Vec3(130, 0, 0), Vec3(0, 0, 105), empty_mat));
    //lights.add(std::make_shared<Sphere>(Vec3(190, 90, 190), 90, empty_mat));

    Camera cam(400, 1., Vec3(278, 278, -800), Vec3(278, 278, 0), 40, 10, 0, 1000, 10);
    cam.set_background(Color(0, 0, 0));
    cam.render(world, lights);
}

void test2()
{
    HittableList world;
    auto ground = std::make_shared<Lambertian>(Color(0.8, 0.5, 0.0));
    //auto center = std::make_shared<Phong>(Color(0.8,0.1,0.2), 0.5, 0.5, 5);
    //auto center = std::make_shared<Dielectric>(1.5);
    auto center = std::make_shared<Lambertian>(Color(0.8,0.1,0.2));
    auto light = std::make_shared<DiffuseLight>(Color(20, 20, 20));

    world.add(std::make_shared<Sphere>(Vec3(0, 0, 0), 0.5, center));
    world.add(std::make_shared<Sphere>(Vec3(0, -100.5, -1), 100, ground));
    world.add(std::make_shared<Quad>(Vec3(0.5, 3, 0.5), Vec3(-0.5, 0, 0), Vec3(0, 0, -0.5), light));

    world = HittableList(std::make_shared<BVHNode>(world));

    HittableList lights;
    auto empty = std::shared_ptr<Material>();
    lights.add(std::make_shared<Quad>(Vec3(0.5, 3, 0.5), Vec3(-0.5, 0, 0), Vec3(0, 0, -0.5), empty));
    //lights.add(std::make_shared<Sphere>(Vec3(0, 0, 0), 0.5, empty));
    //lights.add(std::make_shared<Sphere>(Vec3(0, -100.5, -1), 100, empty));

    Camera cam(400, 16. / 9., Vec3(0, 1, 3), Vec3(0, 0, 0), 90, 10, 0, 1000, 10);
    auto bg = Color(0, 0, 0);
    cam.set_background(bg);
    cam.render(world, lights);
}



int main()
{

    cornell_box();
}