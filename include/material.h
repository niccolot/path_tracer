#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"

using Color = Vec3;

class Material {
    public:
        virtual ~Material() = default;

        virtual bool scatter(
            const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered
        ) const = 0;
}; //class Material 


class Lambertian : public Material {
    private:
        Color albedo;
    
    public:
        Lambertian(const Color& albedo) : albedo(albedo) {}

        bool scatter(
            const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered
        ) const override;
}; // class Lambertian

class Metal : public Material {
    private:
        Color albedo;
    
    public: 
        Metal(const Color& albedo) : albedo(albedo) {}
        bool scatter(
            const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered
        ) const override;
}; // class Metal 
#endif