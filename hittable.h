#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

class HitRecord {
    private:
        Vec3 point_val;
        Vec3 normal_val;
        double t_val;
        bool front_face_val;
    
    public:
        HitRecord() = delete;
        HitRecord(const Vec3& p, const Vec3& n, double t) : point_val(p), normal_val(n), t_val(t) {}
        
        const Vec3& point() const { return point_val; }
        const Vec3& normal() const { return normal_val; }
        double t() const { return t_val; }
        bool front_face() const { return front_face_val; }
        void set_point(const Vec3& p) { point_val = p; }
        void set_t(double t) { t_val = t; }
        void set_face_normal(const Ray& r, const Vec3& outward_normal);
}; // class HitRecord

void HitRecord::set_face_normal(const Ray& r, const Vec3& outward_normal) {
    // outward_normal is assumed to have unit length

    front_face_val = dot(r.direction(), outward_normal) < 0;
    normal_val = front_face_val ? outward_normal : -outward_normal;
}

class Hittable {
    public:
        virtual ~Hittable() = default;

        virtual bool hit(const Ray& r, double ray_tmin, double ray_tmax, HitRecord& rec) const = 0;
}; // class Hittable
#endif