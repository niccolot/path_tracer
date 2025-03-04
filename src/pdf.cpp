#include "pdf.h"
#include "utils.h"

double MixturePDF::value(
    const Vec3& direction_out,
    const Vec3& direction_in,
    const Vec3& direction_view) const {

    //double v0 = p[0] -> value(direction_out, direction_in, direction_view);
    //double v1 = p[1] -> value(direction_out, direction_in, direction_view);
    double v0 = m_p0.value(direction_out, direction_in, direction_view);
    double v1 = m_p1.value(direction_out, direction_in, direction_view);

    return 0.5*v0 + 0.5*v1;
}

Vec3 MixturePDF::generate() const {
    //return RandomUtils::random_double() < 0.5 ? p[0] -> generate() : p[1] -> generate();
    return RandomUtils::random_double() < 0.5 ? m_p0.generate() : m_p1.generate();
}

double PhongPDF::value(
    const Vec3& direction_out,
    const Vec3& direction_in,
    const Vec3& direction_view) const {

    Vec3 reflect_dir = reflect(unit_vector(direction_in), uvw_normal.w());
    double spec_angle = std::fmax(dot(reflect_dir, direction_view), 0.);
    double pdf_specular = ((_n + 1) / (2. * pi)) * std::pow(spec_angle, _n);

    double cos_theta = std::fmax(dot(unit_vector(direction_out), uvw_normal.w()), 0.);
    double pdf_diffuse = cos_theta / pi;

    return _kd * pdf_diffuse + _ks * pdf_specular;
}  

Vec3 PhongPDF::generate() const {
    if (RandomUtils::random_double() < _kd) {
        return uvw_normal.transform(random_cosine_direction());
    } else {
        //return uvw_reflection.transform(random_phong_direction(_n));
        return uvw_normal.transform(random_phong_direction(_n)); 
    }
}