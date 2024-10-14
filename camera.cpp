#include "camera.h"

void Camera::initialize() {
    img_height_val = int(img_width_val/aspect_ratio_val);
    img_height_val = (img_height_val > 1) ? img_height_val : 1; // img_height must be at leat 1 pixel

    center = Vec3();

    // viewport
    auto focal_length = 1.;
    auto viewport_height = 2.;
    auto vieport_width = viewport_height * (double(img_width_val)/img_height_val);
    auto viewport_u = Vec3(vieport_width, 0, 0);
    auto viewport_v = Vec3(0, -viewport_height, 0);

    pixel_delta_u = viewport_u / img_width_val;
    pixel_delta_v = viewport_v / img_height_val;

    auto viewport_upper_left = center - Vec3(0, 0, focal_length) - 0.5*(viewport_u + viewport_v);
    pixel00_loc = viewport_upper_left + 0.5*(pixel_delta_u + pixel_delta_v);
} 

void Camera::write_color(std::ostream& out, const Color& pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    int r_byte = int(255.999 * r);
    int g_byte = int(255.999 * g);
    int b_byte = int(255.999 * b);

    out << r_byte << ' ' << g_byte << ' ' << b_byte << '\n';
}

Color Camera::ray_color(const Ray& r, const Hittable& world) {
    HitRecord rec;
    if (world.hit(r, Interval(0, infinity), rec)) {
        return 0.5*(rec.normal() + Color(1,1,1));
    }

    Vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y() + 1.); // |a| in [0, 1]

    return (1.0-a)*Color(1., 1., 1.) + a*Color(0.5, 0.7, 1.0);
}

void Camera::render(const Hittable& world) {
    initialize();

    std::cout << "P3\n" << img_width_val << ' ' << img_height_val << "\n255\n";

    for (int j=0; j<img_height_val; ++j) {
        std::clog << "\rScanlines remaining: " << (img_height_val - j) << ' ' << std::flush;
        for (int i=0; i<img_width_val; ++i) {
            auto pixel_center = pixel00_loc + (i*pixel_delta_u) + (j*pixel_delta_v);
            auto ray_direction = pixel_center - center;
            Ray r(center, ray_direction);
            auto pixel_color = ray_color(r, world);

            write_color(std::cout, pixel_color);
        }
    }

    std::clog << "\rDone.       \n";
}