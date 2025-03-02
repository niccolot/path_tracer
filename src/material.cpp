#include "material.h"

bool Lambertian::scatter(
    [[maybe_unused]] const Ray& r_in, 
    const HitRecord& rec, 
    scatter_record_t& srec) const {
    
    scattered_rays_t scattered_ray;
    
    // more reflection
    if (RandomUtils::random_double() < r) {
        scattered_ray.attenuation = tex->value(rec.u(), rec.v(), rec.point()) * r;
        scattered_ray.pdf = std::make_shared<CosinePDF>(rec.normal());
        scattered_ray.skip_pdf = false;
        srec.scattered_rays.push_back(scattered_ray);
    } else {
        // more absorption
        scattered_ray.attenuation = tex->value(rec.u(), rec.v(), rec.point()) * (1-r);
        scattered_ray.pdf = std::make_shared<CosinePDF>(rec.normal());
        scattered_ray.skip_pdf = false;
        srec.scattered_rays.push_back(scattered_ray);
    }
    
    return true;
}

double Lambertian::scattering_pdf(
    [[maybe_unused]] const Ray& r_in,
    const HitRecord& rec,
    const Ray& scattered,
    [[maybe_unused]] const Vec3& vdir) const {
    /**
     * @brief cosine distribution for lambertian materials
     */
    double cos_theta = dot(rec.normal(), unit_vector(scattered.direction()));

    return std::fmax(cos_theta/pi, 0.);
}

bool Metal::scatter(
    const Ray& r_in, 
    const HitRecord& rec, 
    scatter_record_t& srec) const {
    
    Vec3 reflected = reflect(r_in.direction(), rec.normal());
    reflected = unit_vector(reflected) + (fuzz * random_unit_vector());

    scattered_rays_t scattered_ray;
    scattered_ray.pdf = nullptr;
    scattered_ray.skip_pdf = true;
    scattered_ray.skip_pdf_ray = Ray(rec.point(), reflected, r_in.time());
    scattered_ray.attenuation = albedo;
    srec.scattered_rays.push_back(scattered_ray);

    return true;
}

double Dielectric::reflectance(double cosine, double eta) {
    // schlick apporximation 
    auto r0 = (1 - eta) / (1 + eta);
    r0 *= r0;

    return r0 + (1-r0)*std::pow((1 - cosine), 5);
}

bool Dielectric::scatter(
    const Ray& r_in, 
    const HitRecord& rec, 
    scatter_record_t& srec) const {

    double ri = rec.front_face() ? (1./eta) : eta;
    Vec3 unit_dir = unit_vector(r_in.direction());

    double cos_theta = std::fmin(dot(-unit_dir, rec.normal()), 1.);
    double sin_theta = std::sqrt(1. - cos_theta*cos_theta);

    // total internal reflection
    bool cannot_refract = ri*sin_theta > 1.0;
    
    // no transmission
    if (cannot_refract || reflectance(cos_theta, ri) > RandomUtils::random_double()) {
        Vec3 direction_reflect;
        scattered_rays_t reflected_ray;

        direction_reflect = reflect(unit_dir, rec.normal());

        reflected_ray.pdf = nullptr;
        reflected_ray.skip_pdf = true;
        reflected_ray.skip_pdf_ray = Ray(rec.point(), direction_reflect, r_in.time());

        // fresnel formulas
        double cos_theta_out = std::fmin(1., dot(rec.normal(), direction_reflect));

        double fr_perp = cos_theta_out - eta*cos_theta / (cos_theta_out + eta*cos_theta);
        fr_perp *= fr_perp;

        double fr_par = eta*cos_theta - cos_theta_out / (eta*cos_theta + cos_theta_out);
        fr_par *= fr_par;

        double fr = 0.5*(fr_par + fr_perp);

        reflected_ray.attenuation = _color * fr;
        srec.scattered_rays.push_back(reflected_ray);
    } else {
        Vec3 direction_reflect, direction_refract;
        scattered_rays_t reflected_ray, refracted_ray;

        direction_reflect = reflect(unit_dir, rec.normal());
        direction_refract = refract(unit_dir, rec.normal(), ri);

        reflected_ray.skip_pdf = true;
        refracted_ray.skip_pdf = true;

        reflected_ray.skip_pdf_ray = Ray(rec.point(), direction_reflect, r_in.time());
        refracted_ray.skip_pdf_ray = Ray(rec.point(), direction_refract, r_in.time());

        // fresnel formulas
        double cos_theta_out = std::fmin(1., dot(rec.normal(), direction_reflect));

        double fr_perp = cos_theta_out - eta*cos_theta / (cos_theta_out + eta*cos_theta);
        fr_perp *= fr_perp;

        double fr_par = eta*cos_theta - cos_theta_out / (eta*cos_theta + cos_theta_out);
        fr_par *= fr_par;

        double fr = 0.5*(fr_par + fr_perp);
        double ft = 1 - fr;

        reflected_ray.attenuation = _color * fr;
        refracted_ray.attenuation = _color * ft;
        
        srec.scattered_rays.push_back(reflected_ray);
        srec.scattered_rays.push_back(refracted_ray);
    }

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
    scatter_record_t& srec) const {

    scattered_rays_t scattered_ray;
    scattered_ray.pdf = std::make_shared<SpherePDF>();
    scattered_ray.skip_pdf = false;
    scattered_ray.attenuation = tex->value(rec.u(), rec.v(), rec.normal());
    srec.scattered_rays.push_back(scattered_ray);

    return true;
}

double Isotropic::scattering_pdf(
    [[maybe_unused]] const Ray& r_in, 
    [[maybe_unused]] const HitRecord& rec,
    [[maybe_unused]] const Ray& scattered,
    [[maybe_unused]] const Vec3& vdir) const {

    return 1 / (4*pi);
}

bool Phong::scatter(
    const Ray& r_in, 
    const HitRecord& rec, 
    scatter_record_t& srec) const {
    
    scattered_rays_t scattered_ray;
    scattered_ray.pdf = std::make_shared<PhongPDF>(rec.normal(), r_in, _kd, _ks, _n);
    scattered_ray.skip_pdf = false;
    scattered_ray.attenuation = tex->value(rec.u(), rec.v(), rec.point());
    srec.scattered_rays.push_back(scattered_ray);

    return true;
}

double Phong::scattering_pdf(
    const Ray& r_in,
    const HitRecord& rec,
    const Ray& scattered,
    const Vec3& vdir) const {
    
    Vec3 reflect_dir = reflect(unit_vector(r_in.direction()), rec.normal());
    double spec_angle = std::fmax(dot(reflect_dir, unit_vector(vdir)), 0.);
    double specular = _ks * ((_n + 1) / (2. * pi)) * std::pow(spec_angle, _n);

    auto cos_theta = dot(rec.normal(), unit_vector(scattered.direction()));
    auto diffuse = _kd * std::fmax(0., cos_theta/pi);

    return diffuse + specular;
}