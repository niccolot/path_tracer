#ifndef PDF_H
#define PDF_H

#include "vec3.h"
#include "onb.h"
#include "hittable.h"

class PDF {
    public:
        virtual ~PDF() {}

        virtual double value(
            const Vec3& direction_out, 
            const Vec3& direction_in,
            const Vec3& direction_view) const = 0;
            
        virtual Vec3 generate() const = 0;
}; // class PDF

class MixturePDF : public PDF {
    private:
        std::shared_ptr<PDF> p[2];

    public:
        MixturePDF(std::shared_ptr<PDF> p0, std::shared_ptr<PDF> p1) {
            p[0] = p0;
            p[1] = p1;
        }

        double value(
            const Vec3& direction_out, 
            [[maybe_unused]] const Vec3& direction_in,
            [[maybe_unused]] const Vec3& direction_view) const override;
        
        Vec3 generate() const override;
}; // class MixturePDF

class SpherePDF : public PDF {
    public:
        SpherePDF() {}

        double value(
            [[maybe_unused]] const Vec3& direction_out, 
            [[maybe_unused]] const Vec3& direction_in,
            [[maybe_unused]] const Vec3& direction_view) const override {
                
            return 1 / (4*pi);
        }

        Vec3 generate() const override {
            return random_unit_vector();
        }
}; // class SpherePDF

class CosinePDF : public PDF {
    private:
        ONB uvw;

    public:
        CosinePDF(const Vec3& w) : uvw(w) {}

        double value(
            const Vec3& direction_out, 
            [[maybe_unused]] const Vec3& direction_in,
            [[maybe_unused]] const Vec3& direction_view) const override {

            auto cos_theta = dot(unit_vector(direction_out), uvw.w());
            
            return std::fmax(0, cos_theta/pi);
        }

        Vec3 generate() const override {
            return uvw.transform(random_cosine_direction());
        }
}; // class CosinePDF

class HittablePDF : public PDF {
    private:
        const Hittable& objects;
        Vec3 origin;

    public:
        HittablePDF(const Hittable& objects, const Vec3& origin) 
            : objects(objects), origin(origin) {}
        
        double value(
            const Vec3& direction_out,
            [[maybe_unused]] const Vec3& direction_in,
            [[maybe_unused]] const Vec3& direction_view) const override {

            return objects.pdf_value(origin, direction_out);
        }

        Vec3 generate() const override {
            return objects.random(origin);
        }
}; // class HittablePDF

class PhongPDF : public PDF {
    private:
        ONB uvw_normal;
        mutable ONB uvw_reflection;
        double _kd;
        double _ks;
        int _n;
    
    public:
        PhongPDF(const Vec3& w, double kd, double ks, int n) :
            uvw_normal(w),
            _kd(kd),
            _ks(ks),
            _n(n) {}

        double value(
            const Vec3& direction_out,
            const Vec3& direction_in,
            const Vec3& direction_view) const override;
        
        Vec3 generate() const override;
}; // class PhongPDF
#endif
