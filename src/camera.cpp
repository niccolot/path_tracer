#include <cmath>

#include "camera.h"
#include "interval.h"
#include "utils.h"
#include "mat3.h"

Camera::Camera(const init_params_t& init_pars, const camera_angles_t& angles) : 
    _init_pars(init_pars), _angles(angles) 
{
    _samples_pp_sqrt = uint32_t(std::sqrt(_init_pars.samples_per_pixel));
    _samples_pp_sqrt_inv = 1.f / float(_samples_pp_sqrt);
    _sampling_scale = 1.f / float(_init_pars.samples_per_pixel);
    _move();

    // image plane
    float h = std::tan(0.5f * degs_to_rads(_init_pars.vfov));
    float img_plane_height = 2.f * h * _init_pars.focus_dist;
    float img_plane_width = img_plane_height * float(_init_pars.img_width) / float(_init_pars.img_height);
    Vec3f img_plane_u = img_plane_width * _u;
    Vec3 img_plane_v = img_plane_height * (-_v);
    _pixel_delta_u = img_plane_u / float(_init_pars.img_width);
    _pixel_delta_v = img_plane_v / float(_init_pars.img_height);
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
    std::cout << "lookfrom: " << _init_pars.lookfrom << "\n";
    std::cout << "theta: " << _angles.theta << " phi: " << _angles.phi << "\n";
    _camera_center = rotate_spherically(_init_pars.lookfrom, _init_pars.lookat, degs_to_rads(_angles.theta), degs_to_rads(_angles.phi));
    std::cout << "camera center: " << _camera_center << "\n";
    _w = unit_vector(_camera_center - _init_pars.lookat); // antiparallel to view direction
    _u = unit_vector(cross(Vec3f(0,1,0), _w)); // perpendicular to view direction and _vup
    _v = cross(_w, _u); // camera up direction in camera frame of reference
    
    // roll, tilt and pan
    _rotate_frame();
}

void Camera::_rotate_frame() {
    _pan_tilt_roll(_w);
    _pan_tilt_roll(_u);
    _pan_tilt_roll(_v);
}

void Camera::_pan_tilt_roll(Vec3f& v) {
    Mat3 roll_rot = roll_rotation(degs_to_rads(_angles.roll));
    Mat3 tilt_rot = tilt_rotation(degs_to_rads(_angles.tilt));
    Mat3 pan_rot = pan_rotation(degs_to_rads(_angles.pan));

    mat_vec_prod_inplace(roll_rot, v);
    mat_vec_prod_inplace(tilt_rot, v);
    mat_vec_prod_inplace(pan_rot, v);
}

std::vector<uint32_t> Camera::render_row(uint32_t j, const std::vector<Sphere>& objects) const {
    /**
     * @brief: where the actual rendering is being done
     */
    std::vector<uint32_t> row_colors;
    row_colors.reserve(_init_pars.img_width);
    for (uint32_t i = 0; i < _init_pars.img_width; ++i) {
        Color pixel_color;
        for (uint32_t sj = 0; sj < _samples_pp_sqrt; ++sj) {
            for (uint32_t si = 0; si < _samples_pp_sqrt; ++si) {
                Ray r = _get_ray(i, j, si, sj);
                pixel_color += _trace(r, _init_pars.depth, objects);
            }
        }
        pixel_color *= _sampling_scale;
        _write_color(pixel_color, row_colors);
    }
    
    return row_colors;
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

Color Camera::_trace(const Ray& r, uint32_t depth, const std::vector<Sphere>& objects) const {
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

bool Camera::_hit(const std::vector<Sphere>& objects, const Ray& r_in, const Interval& ray_t, HitRecord& rec) const {
    /**
     * @brief: calls the hit method on every hittable object in the scene
     */
    HitRecord temp_rec;
    bool hit_anything = false;
    float closest_so_far = ray_t.max();
    for (const auto& obj : objects) {
        if (obj.hit(r_in, Interval(ray_t.min(), closest_so_far), temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.get_t();
            rec = std::move(temp_rec);
        }
    }

    return hit_anything;
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
    row_colors.emplace_back(std::move(pixel));
}

void Camera::_gamma_correction(Color& color) const {
    color.set_x(linear_to_gamma(color.x()));
    color.set_y(linear_to_gamma(color.y()));
    color.set_z(linear_to_gamma(color.z()));
}