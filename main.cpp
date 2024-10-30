#include <iostream>
#include <memory>

#include "camera.h"
#include "sphere.h"
#include "material.h"
#include "bvh.h"
#include "texture.h"
#include "quad.h"
#include "constant_medium.h"

void cornell_box() {
    HittableList world;

    auto red = std::make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = std::make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = std::make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = std::make_shared<DiffuseLight>(Color(7,7,7));

    world.add(std::make_shared<Quad>(Vec3(555,0,0), Vec3(0,555,0), Vec3(0,0,555), green));
    world.add(std::make_shared<Quad>(Vec3(0,0,0), Vec3(0,555,0), Vec3(0,0,555), red));
    world.add(std::make_shared<Quad>(Vec3(0,0,0), Vec3(555,0,0), Vec3(0,0,555), white));
    world.add(std::make_shared<Quad>(Vec3(555,555,555), Vec3(-555,0,0), Vec3(0,0,-555), white));
    world.add(std::make_shared<Quad>(Vec3(0,0,555), Vec3(555,0,0), Vec3(0,555,0), white));
    world.add(std::make_shared<Quad>(Vec3(343,554,332), Vec3(-130,0,0), Vec3(0,0,-105), light));
    
    std::shared_ptr<Metal> alu = std::make_shared<Metal>(Color(0.8, 0.85, 0.88));
    std::shared_ptr<Hittable> box1 = box(Vec3(0,0,0), Vec3(165,330,165), alu);
    std::shared_ptr<Hittable> box2 = box(Vec3(0,0,0), Vec3(165,165,165), white);

    box1 = std::make_shared<RotateY>(box1, 15);
    box1 = std::make_shared<Translate>(box1, Vec3(265,0,265));
    box2 = std::make_shared<RotateY>(box2, -18);
    box2 = std::make_shared<Translate>(box2, Vec3(130,0,65));

    world.add(box1);
    world.add(box2);

    auto empty_mat = std::shared_ptr<Material>();
    Quad lights(Vec3(343,554,332), Vec3(-130,0,0), Vec3(0,0,-105), empty_mat);

    Camera cam(400, 1., Vec3(278,278,-800), Vec3(278,278,0), 40, 10, 0, 50);
    cam.set_background(Color(0,0,0));
    cam.render(world, lights);
}

int main() {
    
    cornell_box();
}