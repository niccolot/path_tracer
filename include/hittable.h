#ifndef HITTABLE_H
#define HITTABLE_H

#include <memory>
#include <vector>
#include "ray.h"
#include "interval.h"

class Material;

class HitRecord {
    private:
        Vec3 point_val;
        Vec3 normal_val;
        double t_val;
        bool front_face_val;
        std::shared_ptr<Material> mat;
    
    public:
        HitRecord() {}
        HitRecord(const Vec3& p, const Vec3& n, double t) : point_val(p), normal_val(n), t_val(t) {}
        
        const Vec3& point() const { return point_val; }
        const Vec3& normal() const { return normal_val; }
        double t() const { return t_val; }
        const std::shared_ptr<Material> const material() { return mat; }
        bool front_face() const { return front_face_val; }
        void set_point(const Vec3& p) { point_val = p; }
        void set_t(double t) { t_val = t; }
        void set_face_normal(const Ray& r, const Vec3& outward_normal);
        void set_material(const std::shared_ptr<Material> material) { mat = material; }
}; // class HitRecord

class Hittable {
    public:
        virtual ~Hittable() = default;

        virtual bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const = 0;
}; // class Hittable

class HittableList : public Hittable {
    private:
        std::vector<std::shared_ptr<Hittable>> objects;
    
    public:
        HittableList() {}
        HittableList(std::shared_ptr<Hittable> object) { add(object); }

        void clear() { objects.clear(); }
        void add(std::shared_ptr<Hittable> object) { objects.push_back(object); }
        bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override;
}; // class HitableList
#endif