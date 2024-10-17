#include "camera.h"
#include "utils.h"
#include "material.h"

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

    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    static const Interval intensity(0.000, 0.999);
    int r_byte = int(255.999 * intensity.clamp(r));
    int g_byte = int(255.999 * intensity.clamp(g));
    int b_byte = int(255.999 * intensity.clamp(b));

    out << r_byte << ' ' << g_byte << ' ' << b_byte << '\n';
}

Color Camera::ray_color(const Ray& r, int depth, const Hittable& world) {
    if (depth <= 0) {
        // no light returned after too many bounces
        return Color(0, 0, 0);
    }

    HitRecord rec;
    double reflectance = 0.7;
    double shadow_acne_offset = 0.001;
    
    // ignore rays that hit the surface nearer than
    // the offset in order to reduce shadow acne 
    if (world.hit(r, Interval(shadow_acne_offset, infinity), rec)) {
        Ray scattered;
        Color attenuation;
        if (rec.material()->scatter(r, rec, attenuation, scattered)) {
            return attenuation * ray_color(scattered, depth-1, world);
        } else {
            return Color(0, 0, 0);
        }
    }

    Vec3 unit_direction = unit_vector(r.direction());
    auto a = reflectance * (unit_direction.y() + 1.); // |a| in [0, 1]

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
                pixel_color += ray_color(r, max_depth, world);
            }

            write_color(std::cout, pixel_color * pixel_samples_scale);
        }
    }

    std::clog << "\nDone.\n";
}