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
    
    /*
    double cos_theta = dot(unit_vector(-direction_in), uvw_normal.w());
    double diffuse = std::fmax(0, cos_theta/pi);
    Vec3 R = reflect(unit_vector(-direction_view), uvw_normal.w());
    uvw_reflection = ONB(R);
    double specular = std::pow(dot(unit_vector(R), unit_vector(direction_in)), _n);

    return _ks*specular + _kd*diffuse;
    */
    
    
    /*
    double diffuse = _kd / pi;

    Vec3 r = unit_vector(reflect(unit_vector(direction_view), uvw.w()));
    double spec_angle = std::fmax(dot(r, unit_vector(-direction_in)), 0.);
    double specular = _ks * ((_n + 2.) / (2. * pi)) * std::pow(spec_angle, _n);

    return specular + diffuse;
    */

    Vec3 dir_out_norm = unit_vector(direction_out);

    Vec3 reflect_dir = reflect(unit_vector(direction_in), uvw_normal.w());
    uvw_reflection = ONB(reflect_dir);
    double spec_angle = std::fmax(dot(reflect_dir, dir_out_norm), 0.);
    double pdf_specular = ((_n + 1) / (2. * pi)) * std::pow(spec_angle, _n);

    double cos_theta = std::fmax(dot(dir_out_norm, uvw_normal.w()), 0.);
    double pdf_diffuse = cos_theta / pi;

    double prob_diffuse = _kd / (_kd + _ks);
    double prob_specular = _ks / (_kd + _ks);

    return prob_diffuse * pdf_diffuse + prob_specular * pdf_specular;
}  

Vec3 PhongPDF::generate() const {
    auto r = random_double();
    double prob_diffuse = _kd / (_kd + _ks);
    if (r < prob_diffuse) {
        return uvw_normal.transform(random_cosine_direction());
    } else {
        return uvw_reflection.transform(random_phong_direction(_n));
    }
    /*
    if (r < prob_diffuse) {
        return uvw_normal.transform(random_cosine_direction());
    } else if (_kd <= r && r < (_kd + _ks)) {
        return uvw_reflection.transform(random_phong_direction(_n));
    } else {
        return Vec3();
    }
    */
        
}