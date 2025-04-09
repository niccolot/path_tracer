#ifndef CAMERA_H
#define CAMERA_H

#include "SDL3/SDL.h"

#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "hittable.h"

typedef struct InitParams {
    uint32_t img_width;
    uint32_t img_height;
    uint32_t window_width;
    uint32_t window_height;
    float vfov; // vertical aperture
    float focus_dist; // distance from camera to image plane
    Vec3f lookfrom;
    Vec3f lookat;
    Color background;
    uint32_t depth;
    uint32_t samples_per_pixel;
    std::string outfile_name;
} init_params_t;

typedef struct CameraAngles {
    float roll = 0.f;
    float tilt = 0.f;
    float pan = 0.f;
    float theta = 90.f;
    float phi = 0.f;
} camera_angles_t;

class Camera {
private:
    bool _gamma_corr = true;
    Vec3f _u, _v, _w; // orthonormal basis
    Vec3f _pixel_delta_u, _pixel_delta_v; // image plane span vectors
    Vec3f _pixel00_loc; // coordinate of top-left pixel 
    Vec3f _camera_center;
    SDL_PixelFormat _pixel_format;
    init_params_t _init_pars;
    camera_angles_t _angles;
    uint32_t _samples_pp_sqrt; // sqrt of samples_per_pixel
    float _samples_pp_sqrt_inv;
    float _sampling_scale;

    void _move();
    void _rotate_frame();
    void _pan_tilt_roll(Vec3f& v);
    Ray _get_ray(uint32_t i, uint32_t j, uint32_t si, uint32_t sj) const;
    Vec3f _sample_square_stratified(uint32_t si, uint32_t sj) const;
    Color _trace(const Ray& r, uint32_t depth, const std::vector<Sphere>& objects) const;
    void _write_color(Color& color, std::vector<uint32_t>& row_colors) const;
    void _gamma_correction(Color& color) const; 
    bool _hit(const std::vector<Sphere>& objects, const Ray& r_in, const Interval& ray_t, HitRecord& rec) const;
    
public:
    Camera() = default;
    Camera(const init_params_t& pars, const camera_angles_t& angles);
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    Camera(Camera&&) = default;
    Camera& operator=(Camera&& other) = default;
    ~Camera() = default;
    
    void set_pixel_format(SDL_PixelFormat format) { _pixel_format = format; }
    std::vector<uint32_t> render_row(uint32_t j, const std::vector<Sphere>& objects) const;
}; // class Camera
#endif