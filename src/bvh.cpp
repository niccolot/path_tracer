#include <algorithm>

#include "bvh.h"

BVHNode::BVHNode(
    std::vector<std::shared_ptr<Hittable>>& objects, 
    size_t start, 
    size_t end) {
    
    int axis = random_int(0,2);

    auto comparator = (axis == 0) ? box_x_compare
                    : (axis == 1) ? box_y_compare
                                  : box_z_compare;
    
    size_t object_span = end - start;

    if (object_span == 1) {
        left = rigth = objects[start];
    } else if (object_span == 2) {
        left = objects[start];
        rigth = objects[start+1];
    } else {
        std::sort(std::begin(objects) + start, std::begin(objects) + end, comparator);
        auto mid = start + 0.5*object_span;
        left = std::make_shared<BVHNode>(objects, start, mid);
        rigth = std::make_shared<BVHNode>(objects, mid, end);
    }

    bbox = AxisAlignedBBox(left->bounding_box(), rigth->bounding_box());
}

bool BVHNode::hit(const Ray& r, Interval ray_t, HitRecord& rec) const {
    if(!bbox.hit(r, ray_t)) {
        return false;
    }
    
    bool hit_left = left->hit(r, ray_t, rec);
    bool hit_rigth = rigth->hit(r, 
        Interval(ray_t.min(), hit_left ? rec.t() : ray_t.max()), rec);

    return hit_left || hit_rigth;
}

bool BVHNode::box_compare(
    const std::shared_ptr<Hittable> a, 
    const std::shared_ptr<Hittable> b, 
    int axis_index
) {

    auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
    auto b_axis_interval = b->bounding_box().axis_interval(axis_index);

    return a_axis_interval.min() < b_axis_interval.min();
}