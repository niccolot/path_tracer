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
    _init_pars.img_width = width;
    _init_pars.img_height = height;
    _init_pars.lookfrom = std::move(lookfrom);
    _init_pars.lookat = std::move(lookat);
    _init_pars.background = std::move(background);
    _init_pars.vfov = vfov;
    _init_pars.focus_dist = focus_dist;
    _init_pars.vup = Vec3f(0.f,1.f,0.f);

    _initialize();
}

void Camera::_initialize() {
    // antiparallel to view direction
    _w = unit_vector(_init_pars.lookfrom - _init_pars.lookat);
    
    // perpendicular to view direction and _vup
    _u = cross(_init_pars.vup, _w);

    // camera up direction in the global frame of reference
    _v = cross(_w, _u);

    // image plane
    float h = std::tan(0.5f * degs_to_rads(_init_pars.vfov));
    float img_plane_height = 2.f * h * _init_pars.focus_dist;
    float img_plane_width = img_plane_height * float(_init_pars.img_width) / float(_init_pars.img_height);
    Vec3f img_plane_u = img_plane_width * _u;
    Vec3 img_plane_v = -img_plane_height * _v;
    _pixel_delta_u = img_plane_u / img_plane_width;
    _pixel_delta_v = img_plane_v / img_plane_height;
    Vec3f img_plane_upper_left = _init_pars.lookfrom - 
                                    _init_pars.focus_dist * _w -
                                    0.5f * (img_plane_u + img_plane_v);
    
    _pixel00_loc = img_plane_upper_left + 0.5f * (_pixel_delta_u + _pixel_delta_v);
}

std::vector<uint32_t> Camera::render_row(uint32_t j, const std::vector<Sphere>& objects) {
    std::vector<uint32_t> row_colors;
    row_colors.reserve(_init_pars.img_width);
    for (uint32_t i = 0; i < _init_pars.img_width; ++i) {
        Color pixel_color;
        Ray r = _get_ray(i, j);
        pixel_color += _trace(r, _init_pars.depth, objects);
        _write_color(pixel_color, row_colors);
    }
    
    return row_colors;
}

Ray Camera::_get_ray(uint32_t i, uint32_t j) {
    Vec3f pixel = _pixel00_loc + (i * _pixel_delta_u) + (j * _pixel_delta_v);

    return Ray{_init_pars.lookfrom, pixel - _init_pars.lookfrom};
}

Color Camera::_trace(const Ray& r, uint32_t depth, const std::vector<Sphere>& objects) {
    if (depth <= 0) {
        return Color();
    }

    HitRecord rec;
    float shadow_acne_offset = 0.001;
    if (!_hit(objects, r, Interval(shadow_acne_offset, inf), rec)) {
        return _init_pars.background;
    }

    Color color_from_scatter = Color();
    color_from_scatter += rec.get_color();

    return color_from_scatter;
}

bool Camera::_hit(const std::vector<Sphere>& objects, const Ray& r_in, const Interval& ray_t, HitRecord& rec) {
    HitRecord temp_rec;
    bool hit_anything = false;
    float closest_so_far = ray_t.max();
    for (const auto& obj : objects) {
        if (obj.hit(r_in, Interval(ray_t.min(), closest_so_far), temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.get_t();
            rec = temp_rec;
        }
    }

    return hit_anything;
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

    uint32_t pixel = SDL_MapRGBA(SDL_GetPixelFormatDetails(_pixel_format), NULL, r_byte, g_byte, b_byte, 0xff);
    row_colors.push_back(std::move(pixel));
}

void Camera::_gamma_correction(Color& color) {
    color.set_x(linear_to_gamma(color.x()));
    color.set_y(linear_to_gamma(color.y()));
    color.set_z(linear_to_gamma(color.z()));
}