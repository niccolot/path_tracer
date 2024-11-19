#ifndef HIT_RECORD_H
#define HIT_RECORD_H

#include <memory>

#include "vec3.h"
#include "ray.h"

class Material;

class HitRecord {
    private:
        Vec3 point_val;
        Vec3 normal_val;
        double t_val;
        double _u;
        double _v;
        bool front_face_val;
        std::shared_ptr<Material> mat;
    
    public:
        HitRecord() {}
        HitRecord(const Vec3& p, const Vec3& n, double t) : point_val(p), normal_val(n), t_val(t) {}
        
        const Vec3& point() const { return point_val; }
        const Vec3& normal() const { return normal_val; }
        double t() const { return t_val; }
        double& u() { return _u; }
        double& v() { return _v; }
        const double& u() const { return _u; }
        const double& v() const { return _v; }
        const std::shared_ptr<Material> material() const { return mat; }
        bool front_face() const { return front_face_val; }

        void set_point(const Vec3& p) { point_val = p; }
        void set_t(double t) { t_val = t; }
        void set_material(const std::shared_ptr<Material> material) { mat = material; }
        void set_u(double u) { _u = u; }
        void set_v(double v) { _v = v; }
        void set_normal(const Vec3& n) { normal_val = n; }
        void set_normal(Vec3&& n) { normal_val = std::move(n); }

        void set_face_normal(const Ray& r, const Vec3& outward_normal) {
            // outward_normal is assumed to have unit length
            front_face_val = dot(r.direction(), outward_normal) < 0;
            normal_val = front_face_val ? outward_normal : -outward_normal;
        }
}; // class HitRecord
#endif