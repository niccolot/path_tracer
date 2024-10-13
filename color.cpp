#include "color.h"
#include "ray.h"

void write_color(std::ostream& out, const Color& pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    int r_byte = int(255.999 * r);
    int g_byte = int(255.999 * g);
    int b_byte = int(255.999 * b);

    out << r_byte << ' ' << g_byte << ' ' << b_byte << '\n';
}


Color ray_color(const Ray& r, const Hittable& world) {
    HitRecord rec;
    if (world.hit(r, 0, infinity, rec)) {
        return 0.5*(rec.normal() + Color(1,1,1));
    }

    Vec3 unit_direction = unit_vector(r.direction());
    auto a = 0.5*(unit_direction.y() + 1.); // |a| in [0, 1]

    return (1.0-a)*Color(1., 1., 1.) + a*Color(0.5, 0.7, 1.0);
}