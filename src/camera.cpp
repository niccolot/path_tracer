#include <cmath>
#include <format>

#include "camera.h"
#include "interval.h"
#include "utils.h"
#include "matrix.h"

Camera::Camera(
    const init_params_t& init_pars, 
    const camera_angles_t& angles, 
    const std::vector<geometry_params_t>& geometries,
    std::shared_ptr<Logger> logger) 
: _init_pars(init_pars), _angles(angles), _geometries(geometries), _logger(logger)
{
    _samples_pp_sqrt = static_cast<uint32_t>(std::sqrt(_init_pars.samples_per_pixel));
    _samples_pp_sqrt_inv = 1.f / static_cast<float>(_samples_pp_sqrt);
    _sampling_scale = 1.f / static_cast<float>(_init_pars.samples_per_pixel);

    // camera frame transformations
    _move();

    // image plane
    float h = std::tan(0.5f * Utils::degs_to_rads(_init_pars.vfov));
    float img_plane_height = 2.f * h * _init_pars.focus_dist;
    float img_plane_width = img_plane_height * static_cast<float>(_init_pars.img_width) / static_cast<float>(_init_pars.img_height);
    Vec3f img_plane_u = img_plane_width * _u;
    Vec3f img_plane_v = img_plane_height * (-_v);
    _pixel_delta_u = img_plane_u / static_cast<float>(_init_pars.img_width);
    _pixel_delta_v = img_plane_v / static_cast<float>(_init_pars.img_height);
    Vec3f img_plane_upper_left = _camera_center - 
                                    (_init_pars.focus_dist * _w) -
                                    0.5f * (img_plane_u + img_plane_v);
    
    _pixel00_loc = img_plane_upper_left + 0.5f * (_pixel_delta_u + _pixel_delta_v);
}

void Camera::_move() {
    /**
     * @brief: moves the camera in 3d space and in it's
     * own frame according to camera angles
     */

    // movement in 3d space
    _camera_center = rotate_spherically(
        _init_pars.lookfrom,
        _init_pars.lookat,
        Utils::degs_to_rads(_angles.theta),
        Utils::degs_to_rads(_angles.phi)
    );
    
    _w = unit_vector(_camera_center - _init_pars.lookat); // antiparallel to view direction
    _u = unit_vector(cross(Vec3f(0,1,0), _w)); // perpendicular to view direction and default up direction (0,1,0)
    _v = cross(_w, _u); // camera up direction in camera frame of reference
    
    // roll, tilt and pan
    _rotate_frame();
}

void Camera::_rotate_frame() {
    /**
     * @brief: rotates the camera's frame of reference
     * @details: tilt is intended as rotation around x axis,
     * pan around y axis and roll around z axis
     */
    Mat3 general_rot = frame_rotation(
        Utils::degs_to_rads(_angles.tilt), 
        Utils::degs_to_rads(_angles.pan), 
        Utils::degs_to_rads(_angles.roll)
    );

    mat_vec_prod_inplace(general_rot, _w);
    mat_vec_prod_inplace(general_rot, _u);
    mat_vec_prod_inplace(general_rot, _v);
}

Ray Camera::_get_ray(uint32_t i, uint32_t j, uint32_t si, uint32_t sj) const {
    /**
     * @brief: samples a ray through pixel (i,j) passing by the subpixel
     * of indices (si,sj) for stratified sampling
     */
    Vec3f offset = _sample_square_stratified(si, sj);
    Vec3f pixel = _pixel00_loc + 
                    ((i + offset.x()) * _pixel_delta_u) + 
                    ((j + offset.y()) * _pixel_delta_v);
    
    return Ray{ _camera_center, pixel - _camera_center };
}

Vec3f Camera::_sample_square_stratified(uint32_t si, uint32_t sj) const {
    /**
     * @brief returns the vector to a random point in the square
     * subpixel specified by grid indices (s_i, s_j) for an 
     * idealized unit square pixel [-0.5, -0.5] x [0.5, 0.5] 
     */
    float px = ((si + RandomUtils::random_float()) * _samples_pp_sqrt_inv) - 0.5;
    float py = ((sj + RandomUtils::random_float()) * _samples_pp_sqrt_inv) - 0.5;

    return Vec3f(px, py, 0);
}

Color Camera::_trace(const Ray& r, uint32_t depth) const {
    if (depth <= 0) {
        return Color();
    }

    HitRecord rec;
    float shadow_acne_offset = 0.001;
    if (!_meshes.hit(r, Interval(shadow_acne_offset, inf), rec)) {
        return _init_pars.background;
    }

    Color color_from_scatter = Color();
    color_from_scatter += rec.get_color();

    return color_from_scatter;
}

void Camera::_write_color(Color& color, std::vector<uint32_t>& row_colors) const {
    /**
     * @brief: processes the row's pixels and put them in the buffer in order to be
     * rendered on the screen by SDL later
     */
    if (_gamma_corr) {
        _gamma_correction(color);
    }
    
    float r = color.x();
    float g = color.y();
    float b = color.z();

    static const Interval intensity(0.000f, 0.999f);
    auto r_byte = uint8_t(intensity.clamp(r) * 255);
    auto g_byte = uint8_t(intensity.clamp(g) * 255);
    auto b_byte = uint8_t(intensity.clamp(b) * 255);

    uint32_t pixel = SDL_MapRGBA(SDL_GetPixelFormatDetails(_pixel_format), NULL, r_byte, g_byte, b_byte, 0xff);
    row_colors.push_back(pixel);
}

void Camera::_gamma_correction(Color& color) const {
    color.set_x(linear_to_gamma(color.x()));
    color.set_y(linear_to_gamma(color.y()));
    color.set_z(linear_to_gamma(color.z()));
}

void Camera::set_meshes() {
    _meshes.set_logger(_logger);
    for (const auto& g : _geometries) {
        objl::Loader loader;
        bool ok = loader.LoadFile("init/meshes/" + g.obj_file);
        if (!ok) {
            throw std::runtime_error{ std::format("failed to load '{}' file", g.obj_file) };
        }

        _meshes.add(loader, g);
    }
}

std::vector<uint32_t> Camera::render_row(uint32_t j) {
    std::vector<uint32_t> row_colors;
    row_colors.reserve(_init_pars.img_width);
    for (uint32_t i = 0; i < _init_pars.img_width; ++i) {
        Color pixel_color;
        for (uint32_t sj = 0; sj < _samples_pp_sqrt; ++sj) {
            for (uint32_t si = 0; si < _samples_pp_sqrt; ++si) {
                Ray r = _get_ray(i, j, si, sj);
                pixel_color += _trace(r, _init_pars.depth);
            }
        }
        
        pixel_color *= _sampling_scale;
        _write_color(pixel_color, row_colors);
    }
    
    return row_colors;
}