#ifndef MATERIAL_H
#define MATERIAL_H

#include "hit_record.h"
#include "texture.h"
#include "color.h"

class Material {
    public:
        virtual ~Material() = default;

        virtual bool scatter(
            const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered
        ) const = 0;

        virtual Color emitted(
            [[maybe_unused]] double u,
            [[maybe_unused]] double v,
            [[maybe_unused]] const Vec3& p) const { return Color(0,0,0); }
}; //class Material 

class Lambertian : public Material {
    private:
        std::shared_ptr<Texture> tex;
    
    public:
        Lambertian(const Color& albedo) : 
            tex(std::make_shared<SolidColor>(albedo)) {}
        
        Lambertian(std::shared_ptr<Texture> tex) : tex(tex) {}

        bool scatter(
            const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered
        ) const override;
}; // class Lambertian

class Metal : public Material {
    private:
        Color albedo;
        double fuzz;
    
    public: 
        Metal(const Color& albedo, double fuzz = 0.) : 
        albedo(albedo), fuzz(fuzz) {}
        bool scatter(
            const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered
        ) const override;
}; // class Metal 

class Dielectric : public Material {
    private:
        // actual refractive index if in vacumm,
        // effective refractive index if embedded in material 
        // eta_eff = eta_material / eta_surrounding
        double refractive_index;

        static double reflectance(double cosine, double refractive_index);
    
    public:
        Dielectric(double eta) : refractive_index(eta) {}

        bool scatter(
            const Ray& r_in, const HitRecord& rec, Color& attenuation, Ray& scattered
        ) const override;
}; // class Dielectric

class DiffuseLight : public Material {
    private:
        std::shared_ptr<Texture> tex;
    
    public: 
        DiffuseLight(std::shared_ptr<Texture> tex) : tex(tex) {}
        DiffuseLight(const Color& emit) : tex(std::make_shared<SolidColor>(emit)) {}
        DiffuseLight(Color&& emit) : tex(std::make_shared<SolidColor>(emit)) {}

        bool scatter(
            [[maybe_unused]] const Ray& r_in,
            [[maybe_unused]] const HitRecord& rec,
            [[maybe_unused]] Color& attenuation,
            [[maybe_unused]] Ray& scattered) const override{
                return false;
            }
            
        Color emitted(double u, double v, const Vec3& p) const override { return tex->value(u,v,p); }
}; // class DiffuseLight
#endif