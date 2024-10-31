#include "material.h"

bool Lambertian::scatter(
    [[maybe_unused]] const Ray& r_in, 
    const HitRecord& rec, 
    ScatterRecord& srec) const {
    
    // reflection
    if (random_double() < r) {
        srec.attenuation = tex->value(rec.u(), rec.v(), rec.point()) * r;
        srec.pdf = std::make_shared<CosinePDF>(rec.normal());
        srec.skip_pdf = false;
    } else {
        // absorption
        srec.attenuation = tex->value(rec.u(), rec.v(), rec.point()) * (1-r);
        srec.pdf = std::make_shared<CosinePDF>(rec.normal());
        srec.skip_pdf = false;
    }
    
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

    return cos_theta < 0 ? 0 : cos_theta/pi;
}

bool Metal::scatter(
    const Ray& r_in, 
    const HitRecord& rec, 
    ScatterRecord& srec) const {
    
    Vec3 reflected = reflect(r_in.direction(), rec.normal());
    reflected = unit_vector(reflected) + (fuzz * random_unit_vector());

    srec.attenuation = albedo;
    srec.pdf = nullptr;
    srec.skip_pdf = true;
    srec.skip_pdf_ray = Ray(rec.point(), reflected, r_in.time());

    return true;
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
    ScatterRecord& srec) const {

    srec.attenuation = Color(1, 1, 1);
    srec.pdf = nullptr;
    srec.skip_pdf = true; 

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

    srec.skip_pdf_ray = Ray(rec.point(), direction, r_in.time());

    return true;
}

Color DiffuseLight::emitted(
    [[maybe_unused]] const Ray& r_int,
    [[maybe_unused]] const HitRecord& rec,
    double u,
    double v,
    const Vec3& p) const { 
    
    // emit light only on one direction
    if (!rec.front_face()) {
        return Color(0,0,0);
    }

    return tex->value(u,v,p); 
}

bool Isotropic::scatter(
    [[maybe_unused]] const Ray& r_in, 
    const HitRecord& rec, 
    ScatterRecord& srec) const {

    srec.attenuation = tex->value(rec.u(), rec.v(), rec.normal());
    srec.pdf = std::make_shared<SpherePDF>();
    srec.skip_pdf = false;

    return true;
}

double Isotropic::scattering_pdf(
    [[maybe_unused]] const Ray& r_in, 
    [[maybe_unused]] const HitRecord& rec,
    [[maybe_unused]] const Ray& scattered) const {

    return 1 / (4*pi);
}

