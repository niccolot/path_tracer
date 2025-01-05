#ifndef MATERIAL_H
#define MATERIAL_H

#include "hit_record.h"
#include "texture.h"
#include "color.h"
#include "onb.h"
#include "pdf.h"
#include "scatter.h"

class Material {
    public:
        virtual ~Material() = default;

        virtual bool scatter(
            const Ray& r_in, 
            const HitRecord& rec, 
            scatter_record_t& srec) const = 0;

        virtual Color emitted(
            [[maybe_unused]] const Ray& r_int,
            [[maybe_unused]] const HitRecord& rec,
            [[maybe_unused]] double u,
            [[maybe_unused]] double v,
            [[maybe_unused]] const Vec3& p) const { return Color(0,0,0); }

        virtual double scattering_pdf(
            const Ray& r_in,
            const HitRecord& rec,
            const Ray& scattered,
            const Vec3& vdir) const = 0;
}; //class Material 

class Lambertian : public Material {
    private:
        std::shared_ptr<Texture> tex;
        double r; // reflectance, r in [0,1]
    
    public:
        Lambertian(const Color& albedo, double r = 1.0) : 
            tex(std::make_shared<SolidColor>(albedo)), r(std::fmin(r,1.)) {}
        
        Lambertian(std::shared_ptr<Texture> tex, double r = 1.0) : tex(tex), r(std::fmin(r,1.)) {}

        void set_reflectance(double refl) { r = std::fmin(refl,1.); }

        bool scatter(
            [[maybe_unused]] const Ray& r_in, 
            const HitRecord& rec, 
            scatter_record_t& srec) const override;

        double scattering_pdf(
            [[maybe_unused]] const Ray& r_in,
            const HitRecord& rec,
            const Ray& scattered,
            [[maybe_unused]] const Vec3& vdir) const override;
}; // class Lambertian

class Metal : public Material {
    private:
        Color albedo;
        double fuzz;
    
    public: 
        Metal(const Color& albedo, double fuzz = 0.) : 
        albedo(albedo), fuzz(fuzz) {}

        bool scatter(
            const Ray& r_in, 
            const HitRecord& rec, 
            scatter_record_t& srec) const override;

        double scattering_pdf(
            [[maybe_unused]] const Ray& r_in,
            [[maybe_unused]] const HitRecord& rec,
            [[maybe_unused]] const Ray& scattered,
            [[maybe_unused]] const Vec3& vdir) const { return 0; }
}; // class Metal 

class Dielectric : public Material {
    private:
        // actual refractive index if in vacumm,
        // effective refractive index if embedded in material 
        // eta_eff = eta_material / eta_surrounding
        double eta;
        Color _color; 
        static double reflectance(double cosine, double eta);
    
    public:
        Dielectric(double eta, const Color& color = Color(1,1,1)) : eta(eta), _color(color) {}

        bool scatter(
            const Ray& r_in, 
            const HitRecord& rec, 
            scatter_record_t& srec) const override;

        double scattering_pdf(
            [[maybe_unused]] const Ray& r_in,
            [[maybe_unused]] const HitRecord& rec,
            [[maybe_unused]] const Ray& scattered,
            [[maybe_unused]] const Vec3& vdir) const { return 0; }
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
            [[maybe_unused]] scatter_record_t& srec) const override {
                return false;
            }

        double scattering_pdf(
            [[maybe_unused]] const Ray& r_in,
            [[maybe_unused]] const HitRecord& rec,
            [[maybe_unused]] const Ray& scattered,
            [[maybe_unused]] const Vec3& vdir) const { return 0; }

        Color emitted(
            [[maybe_unused]] const Ray& r_int,
            [[maybe_unused]] const HitRecord& rec,
            double u,
            double v,
            const Vec3& p) const override;
}; // class DiffuseLight

class Isotropic : public Material {
    private:
        std::shared_ptr<Texture> tex;
    
    public:
        Isotropic(const Color& albedo) : tex(std::make_shared<SolidColor>(albedo)) {}
        Isotropic(std::shared_ptr<Texture> tex) : tex(tex) {}

        bool scatter(
            const Ray& r_in, 
            const HitRecord& rec, 
            scatter_record_t& srec) const override;

        double scattering_pdf(
            [[maybe_unused]] const Ray& r_in,
            [[maybe_unused]] const HitRecord& rec,
            [[maybe_unused]] const Ray& scattered,
            [[maybe_unused]] const Vec3& vdir) const override;
}; //class Isotropic

class Phong : public Material {
    private:
        std::shared_ptr<Texture> tex;

        // must have kd+ks <= 1
        double _kd; // phong model diffuse constant
        double _ks; // phong model specular constant
        int _n; // phong model exponent
        
    public:
        Phong(
            std::shared_ptr<Texture> tex,
            double kd = 0.2,
            double ks = 0.8,
            int n = 10) : 
                tex(tex),
                _kd(kd),
                _ks(ks),
                _n(n) {}

        Phong(
            const Color& albedo,
            double kd = 0.2,
            double ks = 0.8,
            int n = 10) : Phong(std::make_shared<SolidColor>(albedo), kd, ks, n) {}
        
        void set_n(int n) {_n = n; }
        void set_kd(double kd) { 
            _kd = kd;
            _ks = 1. - kd;  
        }

        bool scatter(
            const Ray& r_in, 
            const HitRecord& rec, 
            scatter_record_t& srec) const override;

        double scattering_pdf(
            const Ray& r_in,
            const HitRecord& rec,
            const Ray& scattered,
            const Vec3& vdir) const override;        
}; // class Phong
#endif