#ifndef HITTABLE_H
#define HITTABLE_H

#include <memory>
#include <vector>

#include "hit_record.h"
#include "ray.h"
#include "interval.h"
#include "axis_aligned_bbox.h"


class Hittable {
    public:
        virtual ~Hittable() = default;

        virtual bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const = 0;
        virtual const AxisAlignedBBox& bounding_box() const = 0;
        virtual double pdf_value(const Vec3& origin, const Vec3& direction) const = 0;
        virtual Vec3 random(const Vec3& origin) const = 0;
}; // class Hittable

class HittableList : public Hittable {
    private:
        std::vector<std::shared_ptr<Hittable>> objects;
        AxisAlignedBBox bbox;
    
    public:
        HittableList() {}
        HittableList(std::shared_ptr<Hittable> object) { add(object); }

        std::vector<std::shared_ptr<Hittable>>& get_objects() { return objects; }
        const AxisAlignedBBox& bounding_box() const override { return bbox; }; 

        void clear() { objects.clear(); }
        void add(std::shared_ptr<Hittable> object);
        bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override;
        double pdf_value(
            [[maybe_unused]] const Vec3& origin, 
            [[maybe_unused]] const Vec3& direction) const override{ return 0; }
        
        Vec3 random([[maybe_unused]] 
            const Vec3& origin) const override { return Vec3(1,0,0); }
}; // class HitableList

class Translate : public Hittable {
    private:
        std::shared_ptr<Hittable> object;
        Vec3 offset;
        AxisAlignedBBox bbox;
    
    public:
        Translate(
            std::shared_ptr<Hittable> object,
            const Vec3& offset) :

                object(object), offset(offset) { 
                    bbox = object->bounding_box() + offset; 
        }

        const AxisAlignedBBox& bounding_box() const override { return bbox; }

        bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override;
        double pdf_value(
            [[maybe_unused]] const Vec3& origin, 
            [[maybe_unused]] const Vec3& direction) const override{ return 0; }
        
        Vec3 random(
            [[maybe_unused]] const Vec3& origin) const override { return Vec3(1,0,0); }
 }; // class Translate 

 class RotateY : public Hittable {
    private:
        std::shared_ptr<Hittable> object;
        double sin_theta;
        double cos_theta;
        AxisAlignedBBox bbox;
    
    public:
        RotateY(std::shared_ptr<Hittable> object, double angle);

        bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override;
        const AxisAlignedBBox& bounding_box() const override { return bbox; }
        double pdf_value(
            [[maybe_unused]] const Vec3& origin, 
            [[maybe_unused]] const Vec3& direction) const override{ return 0; }
        
        Vec3 random(
            [[maybe_unused]] const Vec3& origin) const override { return Vec3(1,0,0); }
 }; // class RotateY
#endif