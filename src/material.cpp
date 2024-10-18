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

double Dielectric::reflectance(double cosine, double refractive_index) {
    // schlick apporximation for reflectance
    auto r0 = (1 - refractive_index) / (1 + refractive_index);
    r0 *= r0;

    return r0 + (1-r0)*std::pow((1 - cosine), 5);
}

bool Dielectric::scatter(
    const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered
) const {

    attenuation = Color(1, 1, 1);
    double ri = rec.front_face() ? (1./refractive_index) : refractive_index;
    Vec3 unit_dir = unit_vector(r_in.direction());

    double cos_theta = std::fmin(dot(-unit_dir, rec.normal()), 1.);
    double sin_theta = std::sqrt(1. - cos_theta*cos_theta);
    Vec3 direction;

    // total internal reflection
    bool cannot_reflect = sin_theta > 1.0;
    
    if (cannot_reflect || reflectance(cos_theta, ri) > random_double()) {
        direction = reflect(unit_dir, rec.normal());
    } else {
        direction = refract(unit_dir, rec.normal(), ri);
    }

    scattered = Ray(rec.point(), direction);

    return true;
}

