#include "pdf.h"

double MixturePDF::value(
    const Vec3& direction_out,
    const Vec3& direction_in,
    const Vec3& direction_view) const {
    
    double v0{};
    double v1{};
    
    if (p[0] != nullptr) {
        v0 = p[0]->value(direction_out, direction_in, direction_view);
    }

    if (p[1] != nullptr) {
        v1 = p[1]->value(direction_out, direction_in, direction_view);
    } 

    return 0.5 * v0 + 0.5 * v1;
}

Vec3 MixturePDF::generate() const {
    bool p0_null = p[0] == nullptr;
    bool p1_null = p[1] == nullptr;

    int state = (p0_null ? 1 : 0) | (p1_null ? 2 : 0);

    switch (state) {
        case (3): // (nullptr, nullptr)
            return random_unit_vector();
        
        case (2): // (ok, nullptr)
            return p[0]->generate();

        case (1): // (nullptr, ok)
            return p[1]->generate();

        default: // (ok, ok)
            return random_double() < 0.5 ? p[0]->generate() : p[1]->generate();
        } 
}

double PhongPDF::value(
    const Vec3& direction_out,
    const Vec3& direction_in,
    const Vec3& direction_view) const {
    /*
    auto cos_theta = dot(unit_vector(direction_out), uvw.w());
    auto reflection = std::fmax(0, cos_theta/pi);
    auto R = reflect(direction_in, uvw.w());
    auto specular = std::pow(dot(-R, -direction_view), _n);

    return _ks*specular + _kd*reflection;
    */
    auto cos_theta = dot(unit_vector(direction_out), uvw.w());
    return std::fmax(0, cos_theta/pi);
}  

Vec3 PhongPDF::generate() const {
    /*
    auto r = random_double();
    if (r < _kd) {
        return uvw.transform(random_cosine_direction());
    } else if (_kd <= r && r < _kd + _ks) {
        return uvw.transform(random_phong_direction(_n));
    } else {
        return Vec3();
    }
    */
    return uvw.transform(random_cosine_direction());
}