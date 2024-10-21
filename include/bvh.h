#ifndef BVH_H
#define BVH_H

#include "hittable.h"

class BVHNode : public Hittable {
    private:
        std::shared_ptr<Hittable> left;
        std::shared_ptr<Hittable> rigth;
        AxisAlignedBBox bbox;

        static bool box_compare(
            const std::shared_ptr<Hittable> a, 
            const std::shared_ptr<Hittable> b, 
            int axis_index
        );

        static bool box_x_compare(
            const std::shared_ptr<Hittable> a, 
            const std::shared_ptr<Hittable> b
        ) { return box_compare(a,b,0); }

        static bool box_y_compare(
            const std::shared_ptr<Hittable> a, 
            const std::shared_ptr<Hittable> b
        ) { return box_compare(a,b,1); }

        static bool box_z_compare(
            const std::shared_ptr<Hittable> a, 
            const std::shared_ptr<Hittable> b
        ) { return box_compare(a,b,2); }

    public:
        BVHNode(HittableList& list) : 
            BVHNode(list.get_objects(), 0, list.get_objects().size()) {}
        
        BVHNode(
            std::vector<std::shared_ptr<Hittable>>& objects, 
            size_t start, 
            size_t end);
        
        const AxisAlignedBBox& bounding_box() const override { return bbox; }
        bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override;
}; // class BVHNode
#endif