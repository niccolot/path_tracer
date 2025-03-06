#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "hittable.h"
#include "texture.h"
#include "material.h"

class ConstantMedium : public Hittable{
    private:
        std::shared_ptr<Hittable> boundary;
        double neg_inv_density;
        std::shared_ptr<Material> phase_function;

    public:
        ConstantMedium(
            std::shared_ptr<Hittable> boundary,
            double density,
            std::shared_ptr<Texture> text
        ) : 
            boundary(boundary),
            neg_inv_density(-(1/density)),
            phase_function(std::make_shared<Isotropic>(text)) {}

        ConstantMedium(
            std::shared_ptr<Hittable> boundary,
            double density,
            const Color& albedo
        ) :
            boundary(boundary),
            neg_inv_density(-(1/density)),
            phase_function(std::make_shared<Isotropic>(albedo)) {}

        bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override;
        const AxisAlignedBBox& bounding_box() const override { return boundary->bounding_box(); }
}; // class ConstantMedium
#endif