#include "material.h"

bool Lambertian::scatter(
    [[maybe_unused]] const Ray& r_in, 
    const HitRecord& rec, 
    Color& attenuation, 
    Ray& scattered,
    double& pfd) const {
    
    ONB uvw(rec.normal());
    auto scatter_direction = uvw.transform(random_cosine_direction());

    // catch degenerate rays
    if (scatter_direction.near_zero()) {
        scatter_direction = rec.normal();
    }

    scattered = Ray(rec.point(), unit_vector(scatter_direction), r_in.time());
    attenuation = tex->value(rec.u(), rec.v(), rec.point());
    pfd = dot(uvw.w(), scattered.direction()) / pi;

    return true;
}

double Lambertian::scattering_pdf(
    [[maybe_unused]] const Ray& r_in,
    const HitRecord& rec,
    const Ray& scattered) const {
    /**
     * @brief cosine distribution for lambertian materials
     */
    auto cos_theta = dot(rec.normal(), unit_vector(scattered.direction()));

    return cos_theta < 0 ? 0 : cos_theta;
}

bool Metal::scatter(
    const Ray& r_in, 
    const HitRecord& rec, 
    Color& attenuation, 
    Ray& scattered,
    [[maybe_unused]] double& pdf) const {
    
    Vec3 reflected = reflect(r_in.direction(), rec.normal());
    reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
    scattered = Ray(rec.point(), reflected, r_in.time());
    attenuation = albedo;

    // if due to fuzziness the incident ray is scattered below 
    // the surface, the ray is fully absorbed
    return (dot(scattered.direction(), rec.normal()) > 0);
}

double Dielectric::reflectance(double cosine, double refractive_index) {
    // schlick apporximation 
    auto r0 = (1 - refractive_index) / (1 + refractive_index);
    r0 *= r0;

    return r0 + (1-r0)*std::pow((1 - cosine), 5);
}

bool Dielectric::scatter(
    const Ray& r_in, 
    const HitRecord& rec, 
    Color& attenuation, 
    Ray& scattered,
    [[maybe_unused]] double& pdf) const {

    attenuation = Color(1, 1, 1);
    double ri = rec.front_face() ? (1./refractive_index) : refractive_index;
    Vec3 unit_dir = unit_vector(r_in.direction());

    double cos_theta = std::fmin(dot(-unit_dir, rec.normal()), 1.);
    double sin_theta = std::sqrt(1. - cos_theta*cos_theta);
    Vec3 direction;

    // total internal reflection
    bool cannot_refract = ri*sin_theta > 1.0;
    
    if (cannot_refract || reflectance(cos_theta, ri) > random_double()) {
        direction = reflect(unit_dir, rec.normal());
    } else {
        direction = refract(unit_dir, rec.normal(), ri);
    }

    scattered = Ray(rec.point(), direction, r_in.time());

    return true;
}

bool Isotropic::scatter(
    const Ray& r_in, 
    const HitRecord& rec, 
    Color& attenuation, 
    Ray& scattered,
    double& pdf) const {

    scattered = Ray(rec.point(), random_unit_vector(), r_in.time());
    attenuation = tex->value(rec.u(), rec.v(), rec.point());
    pdf = 1 / (4*pi);

    return true;
}

double Isotropic::scattering_pdf(
    const Ray& r_in [[maybe_unused]], 
    const HitRecord& rec [[maybe_unused]],
    const Ray& scattered [[maybe_unused]]) const {

        return 1 / (4*pi);
    }

