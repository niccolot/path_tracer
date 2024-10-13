#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include <memory>

#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "sphere.h"
#include "hittable.h"

void render_image(std::ostream& out, int img_width, double aspect_ratio) {
    // image
    int img_height = int(img_width / aspect_ratio);
    img_height = (img_height > 1) ? img_height : 1; // img_height has to be at least 1 pixel

    // world
    HittableList world;
    world.add(std::make_shared<Sphere>(Vec3(0, -100.5, -1), 100));
    world.add(std::make_shared<Sphere>(Vec3(0, 0, -1), 0.5));
    

    // camera
    auto focal_length = 1.0;
    auto viewport_height = 2.0;
    auto viewport_width = viewport_height * (double(img_width)/img_height);
    auto camera_center = Vec3();

    // vectors across horizontal and vertical viewport edges
    auto viewport_u = Vec3(viewport_width, 0, 0);
    auto viewport_v = Vec3(0, -viewport_height, 0);

    // horizontal and vertical delta-pixel vectors
    auto pixel_delta_u = viewport_u / img_width;
    auto pixel_delta_v = viewport_v / img_height;

    // upperleft pixel
    auto viewport_upper_left = camera_center - Vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
    auto pixel00_loc = viewport_upper_left + 0.5*(pixel_delta_u + pixel_delta_v);

    out << "P3\n" << img_width << ' ' << img_height << "\n255\n";

    for (int j=0; j<img_height; ++j) {
        std::clog << "\rScanlines remaining: " << (img_height - j) << ' ' << std::flush;
        for (int i=0; i<img_width; ++i) {
            auto pixel_center = pixel00_loc + (i*pixel_delta_u) + (j*pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            Ray r(camera_center, ray_direction);
            auto pixel_color = ray_color(r, world);

            write_color(out, pixel_color);
        }
    }

    std::clog << "\rDone.       \n";
}
#endif