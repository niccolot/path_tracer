#include <algorithm>

#include "bvh.h"

BVHNode::BVHNode(
    std::vector<std::shared_ptr<Hittable>>& objects, 
    size_t start, 
    size_t end) {
    
    bbox = AxisAlignedBBox::empty();
    for (size_t object_index=start; object_index<end; ++object_index) {
        bbox = AxisAlignedBBox(bbox, objects[object_index]->bounding_box());
    }

    int axis = bbox.longest_axis();

    auto comparator = (axis == 0) ? box_x_compare
                    : (axis == 1) ? box_y_compare
                                  : box_z_compare;
    
    size_t object_span = end - start;

    if (object_span == 1) {
        left = right = objects[start];
    } else if (object_span == 2) {
        left = objects[start];
        right = objects[start+1];
    } else {
        auto mid = start + 0.5*object_span;
        std::nth_element(objects.begin() + start, objects.begin() + mid, objects.begin() + end);
        left = std::make_shared<BVHNode>(objects, start, mid);
        right = std::make_shared<BVHNode>(objects, mid, end);
    }
}

bool BVHNode::hit(const Ray& r, Interval ray_t, HitRecord& rec) const {
    if(!bbox.hit(r, ray_t)) {
        return false;
    }
    
    bool hit_left = left->hit(r, ray_t, rec);
    bool hit_right = right->hit(r, 
        Interval(ray_t.min(), hit_left ? rec.t() : ray_t.max()), rec);

    return hit_left || hit_right;
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