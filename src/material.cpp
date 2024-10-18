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
    reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
    scattered = Ray(rec.point(), reflected);
    attenuation = albedo;

    // if due to fuzziness the incident ray is scattered below 
    // the surface, the ray is fully absorbed
    return (dot(scattered.direction(), rec.normal()) > 0);
}

bool Dielectric::scatter(
    const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered
) const {

    attenuation = Color(1, 1, 1);
    double ri = rec.front_face() ? (1./refractive_index) : refractive_index;

    Vec3 unit_dir = unit_vector(r_in.direction());
    Vec3 refracted = refract(unit_dir, rec.normal(), ri);
    scattered = Ray(rec.point(), refracted);

    return true;
}

