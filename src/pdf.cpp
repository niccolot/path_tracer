#include "pdf.h"

double MixturePDF::value(
    const Vec3& direction_out,
    const Vec3& direction_in,
    const Vec3& direction_view) const {

    double v0 = p[0]->value(direction_out, direction_in, direction_view);
    double v1 = p[1]->value(direction_out, direction_in, direction_view); 
    
    return 0.5*v0 + 0.5*v1;
    
}

Vec3 MixturePDF::generate() const {
    return random_double() < 0.5 ? p[0]->generate() : p[1]->generate();
}

double PhongPDF::value(
    const Vec3& direction_out,
    const Vec3& direction_in,
    const Vec3& direction_view) const {
    
    auto cos_theta = dot(-direction_in, uvw.w());
    auto diffuse = std::fmax(0, cos_theta);
    auto R = reflect(unit_vector(direction_in), uvw.w());
    auto specular = std::pow(dot(unit_vector(R), unit_vector(-direction_view)), _n);

    return _ks*specular + _kd*diffuse;
}  

Vec3 PhongPDF::generate() const {
    
    auto r = random_double();
    if (r < _kd) {
        return uvw.transform(random_cosine_direction());
    } else if (_kd <= r && r < _kd + _ks) {
        return uvw.transform(random_phong_direction(_n));
    } else {
        return Vec3();
    }    
}