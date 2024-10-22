#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>

#include "color.h"
#include "vec3.h"
#include "perlin.h"

class Texture {
    public:
        virtual ~Texture() = default;
        virtual Color value(double u, double v, const Vec3& p) = 0;
}; // class Texture

class SolidColor : public Texture {
    private:
        Color albedo;
    
    public:
        SolidColor(const Color& albedo) : albedo(albedo) {}
        SolidColor(double r, double g, double b) : SolidColor(Color(r,g,b)) {}
        
        Color value(
            [[maybe_unused]] double u, 
            [[maybe_unused]] double v, 
            [[maybe_unused]] const Vec3& p) override { return albedo; }
}; // class SolidColor

class CheckerTexture : public Texture {
    private:
        double inv_scale;
        std::shared_ptr<Texture> even;
        std::shared_ptr<Texture> odd;
    
    public:
        CheckerTexture(
            double scale, 
            std::shared_ptr<Texture> even,
            std::shared_ptr<Texture> odd) :
                inv_scale(1./scale),
                even(even),
                odd(odd) {}
        
        CheckerTexture(
            double scale,
            const Color& c1,
            const Color& c2) :
                CheckerTexture(
                    scale,
                    std::make_shared<SolidColor>(c1),
                    std::make_shared<SolidColor>(c2)) {}
        
        Color value(double u, double v, const Vec3& p) override;
}; // class CheckerTexture

class NoiseTexture : public Texture {
    private:
        Perlin noise;
            
    public:
        NoiseTexture() {}
        
        Color value(
            [[maybe_unused]] double u, 
            [[maybe_unused]] double v,
            [[maybe_unused]]  const Vec3& p) override {

                return Color(1,1,1) * noise.noise(p);
            }
}; // class NoiseTexture
#endif