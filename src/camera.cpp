#include <cmath>

#include "camera.h"
#include "interval.h"
#include "utils.h"

Camera::Camera(const init_params_t& init_pars) :
    Camera(
        init_pars.img_width,
        init_pars.img_height,
        init_pars.lookfrom,
        init_pars.lookat,
        init_pars.background,
        init_pars.vfov,
        init_pars.focus_dist) {}

Camera::Camera(
    uint32_t width,
    uint32_t height,
    Vec3f lookfrom,
    Vec3f lookat,
    Color background,
    float vfov,
    float focus_dist
)  {   
    _img_width = width;
    _img_height = height;
    _lookfrom = std::move(lookfrom);
    _lookat = std::move(lookat);
    _background = std::move(background);
    _vfov = vfov;
    _focus_dist = focus_dist;

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

std::vector<uint32_t> Camera::render_row(uint32_t row) {
    std::vector<uint32_t> row_colors;
    row_colors.reserve(_img_width);
    for (uint32_t i = 0; i < _img_width; ++i) {
        Color pixel_color;
        Ray r = _get_ray(i, row);
        pixel_color += _ray_color(r);
        _write_color(pixel_color, row_colors);
    }
    
    return row_colors;
}

Ray Camera::_get_ray(uint32_t i, uint32_t j) {
    Vec3f pixel = _pixel00_loc + (i * _pixel_delta_u) + (j * _pixel_delta_v);

    return Ray{_lookfrom, pixel - _lookfrom};
}

Color Camera::_ray_color(const Ray& r) {
    float a = 0.5f * (unit_vector(r.direction()).y() + 1.f);

    return (1.f - a) * Color(1.f,1.f,1.f) + a * _background;
}

void Camera::_write_color(Color& color, std::vector<uint32_t>& row_colors) {
    if (_gamma_corr) {
        _gamma_correction(color);
    }
    
    float r = color.x();
    float g = color.y();
    float b = color.z();

    static const Interval intensity(0.000f, 0.999f);
    auto r_byte = uint8_t(intensity.clamp(r) * 256);
    auto g_byte = uint8_t(intensity.clamp(g) * 256);
    auto b_byte = uint8_t(intensity.clamp(b) * 256);

    uint32_t pixel = SDL_MapRGBA(SDL_GetPixelFormatDetails(_pixel_format), NULL, r_byte, g_byte, b_byte, 255);
    row_colors.push_back(std::move(pixel));
}

void Camera::_gamma_correction(Color& color) {
    color.set_x(linear_to_gamma(color.x()));
    color.set_y(linear_to_gamma(color.y()));
    color.set_z(linear_to_gamma(color.z()));
}