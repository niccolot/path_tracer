#include <cmath>

#include "camera.h"
#include "interval.h"
#include "utils.h"

Camera::Camera(
    uint32_t width,
    float aspect_ratio,
    Vec3f lookfrom,
    Vec3f lookat,
    Color background,
    float vfov,
    float focus_dist
) :
    _img_width(width),
    _aspect_ratio(aspect_ratio),
    _lookfrom(lookfrom),
    _lookat(lookat),
    _background(background),
    _vfov(vfov),
    _focus_dist(focus_dist)
{
    _img_height = uint32_t(_img_width / _aspect_ratio);
    _img_height = (_img_height > 1) ? _img_height : 1; // img_height at leas 1 pixel
    _vup = Vec3f(0.f,1.f,0.f);

    // antiparallel to view direction
    _w = unit_vector(_lookfrom - _lookat);
    
    // perpendicular to view direction and _vup
    _u = cross(_vup, _w);

    // camera up direction in the global frame of reference
    _v = cross(_w, _u);

    // image plane
    float h = std::tan(0.5f * degs_to_rads(_vfov));
    float img_plane_height = 2.f * h * _focus_dist;
    float img_plane_width = img_plane_height * float(_img_width) / float(_img_height);
    Vec3f img_plane_u = img_plane_width * _u;
    Vec3 img_plane_v = -img_plane_height * _v;
    _pixel_delta_u = img_plane_u / img_plane_width;
    _pixel_delta_v = img_plane_v / img_plane_height;
    Vec3f img_plane_upper_left = _lookfrom - 
                                    _focus_dist * _w -
                                    0.5f * (img_plane_u + img_plane_v);
    
    _pixel00_loc = img_plane_upper_left + 0.5f * (_pixel_delta_u + _pixel_delta_v);
}

void Camera::render() {
    std::cout << "P3\n" << _img_width << ' ' << _img_height << "\n255\n";
    for(uint32_t j = 0; j < _img_height; ++j) {
        for (uint32_t i = 0; i < _img_width; ++i) {
            Color pixel_color;
            Ray r = get_ray(i, j);
            pixel_color += ray_color(r);
            write_color(pixel_color);
        }
    }
}

Ray Camera::get_ray(uint32_t i, uint32_t j) {
    Vec3f pixel = _pixel00_loc + (i * _pixel_delta_u) + (j * _pixel_delta_v);

    return Ray{_lookfrom, pixel - _lookfrom};
}

Color Camera::ray_color(const Ray& r) {
    float a = 0.5f * (unit_vector(r.direction()).y() + 1.f);

    return (1.f - a) * Color(1.f,1.f,1.f) + a * _background;
}

void Camera::write_color(Color& color) {
    if (_gamma_correction) {
        gamma_correction(color);
    }
    
    float r = color.x();
    float g = color.y();
    float b = color.z();

    static const Interval intensity(0.000f, 0.999f);
    auto r_byte = uint32_t(intensity.clamp(r) * 256);
    auto g_byte = uint32_t(intensity.clamp(g) * 256);
    auto b_byte = uint32_t(intensity.clamp(b) * 256);

    std::cout << r_byte << ' ' << g_byte << ' ' << b_byte << '\n';
}

void Camera::gamma_correction(Color& color) {
    color.set_x(linear_to_gamma(color.x()));
    color.set_y(linear_to_gamma(color.y()));
    color.set_z(linear_to_gamma(color.z()));
}