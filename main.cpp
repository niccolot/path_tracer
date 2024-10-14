#include <iostream>
#include <memory>

#include "camera.h"
#include "hittable.h"
#include "sphere.h"

int main() {

    int img_width = 1024;
    double aspect_ratio = 16.0 / 9.0;

    HittableList world;

    world.add(std::make_shared<Sphere>(Vec3(0, 0, -1), 0.5));
    world.add(std::make_shared<Sphere>(Vec3(0, -100.5, -1), 100));

    Camera cam;
    cam.set_image_width(img_width);
    cam.set_aspect_ratio(aspect_ratio);

    cam.render(world);
}