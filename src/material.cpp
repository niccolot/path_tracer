#include "material.h"

bool Lambertian::scatter(
    [[maybe_unused]] const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered
) const {
    auto scatter_direction = rec.normal() + random_unit_vector();

    // catch degenerate rays
    if (scatter_direction.near_zero()) {
        scatter_direction = rec.normal();
    }

    scattered = Ray(rec.point(), scatter_direction);
    attenuation = albedo;

    return true;
}

bool Metal::scatter(
    const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered
) const {
    Vec3 reflected = reflect(r_in.direction(), rec.normal());
    scattered = Ray(rec.point(), reflected);
    attenuation = albedo;

    return true;
}

