#include "camera.h"
#include "utils.h"

void Camera::initialize() {
    img_height_val = int(img_width_val/aspect_ratio_val);
    img_height_val = (img_height_val > 1) ? img_height_val : 1; // img_height must be at leat 1 pixel

    pixel_samples_scale = 1.0 / samples_per_pixel;

    center = Vec3();

    // viewport
    auto focal_length = 1.;
    auto viewport_height = 2.;
    auto vieport_width = viewport_height * (double(img_width_val)/img_height_val);
    auto viewport_u = Vec3(vieport_width, 0, 0);
    auto viewport_v = Vec3(0, -viewport_height, 0);

    pixel_delta_u = viewport_u / img_width_val;
    pixel_delta_v = viewport_v / img_height_val;

    auto viewport_upper_left = center - 
                                Vec3(0, 0, focal_length) 
                                - 0.5*(viewport_u + viewport_v);
    pixel00_loc = viewport_upper_left + 0.5*(pixel_delta_u + pixel_delta_v);
} 

Ray Camera::get_ray(int i, int j) const {
    /**
     * @brief construct a camera ray originated from
     * the camera center and directed at a randomly 
     * sampled point around pixel (i, j)
     */

    auto offset = sample_square();
    auto pixel_sample = pixel00_loc 
                        + ((i + offset.x()) * pixel_delta_u)
                        + ((j + offset.y()) * pixel_delta_v);
    
    auto ray_direction = pixel_sample - center;

    return Ray(center, ray_direction);
}

void Camera::write_color(std::ostream& out, const Color& pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    static const Interval intensity(0.000, 0.999);
    int r_byte = int(255.999 * intensity.clamp(r));
    int g_byte = int(255.999 * intensity.clamp(g));
    int b_byte = int(255.999 * intensity.clamp(b));

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

            Color pixel_color(0, 0, 0);
            for (int sample = 0; sample < samples_per_pixel; ++sample) {
                Ray r = get_ray(i,j);
                pixel_color += ray_color(r, world);
            }

            write_color(std::cout, pixel_color * pixel_samples_scale);
        }
    }

    std::clog << "\nDone.\n";
}