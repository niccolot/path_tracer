#ifndef BVH_H
#define BVH_H

#include "hittable.h"

class BVHNode : public Hittable {
    private:
        std::shared_ptr<Hittable> left;
        std::shared_ptr<Hittable> right;
        AxisAlignedBBox bbox;
        
    public:
        BVHNode(HittableList& list) : 
            BVHNode(list.get_objects(), 0, list.get_objects().size()) {}
        
        BVHNode(
            std::vector<std::shared_ptr<Hittable>>& objects, 
            size_t start, 
            size_t end);
        
        const AxisAlignedBBox& bounding_box() const override { return bbox; }
        bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override;
        double pdf_value(
            [[maybe_unused]] const Vec3& origin, 
            [[maybe_unused]] const Vec3& direction) const override{ return 0; }
        
        Vec3 random(
            [[maybe_unused]]const Vec3& origin) const override { return Vec3(1,0,0); }
}; // class BVHNode
#endif