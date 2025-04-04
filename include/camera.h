#ifndef CAMERA_H
#define CAMERA_H

#include <cstdint>

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
    Vec3f vup; // up direction in camera frame of reference
    uint32_t depth;
    std::string outfile_name;
} init_params_t;

class Camera {

private:
    bool _gamma_corr = true;
    Vec3f _u, _v, _w; // orthonormal basis
    Vec3f _pixel_delta_u, _pixel_delta_v; // image plane span vectors
    Vec3f _pixel00_loc; // coordinate of top-left pixel 
    SDL_PixelFormat _pixel_format;
    init_params_t _init_pars;

    Ray _get_ray(uint32_t i, uint32_t j);
    Color _trace(const Ray& r, uint32_t depth, const std::vector<Sphere>& objects);
    void _write_color(Color& color, std::vector<uint32_t>& row_colors);
    void _gamma_correction(Color& color); 
    void _initialize();
    
public:
    Camera() = default;
    Camera(
        uint32_t width,
        uint32_t height,
        Vec3f lookfrom = Vec3f(),
        Vec3f lookat = Vec3f(0.f, 0.f, -1.f),
        Color background = Color(),
        float vfov = 90.f,
        float focus_dist = 10.f
    );

    Camera(const init_params_t& pars);
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    Camera(Camera&&) = default;
    Camera& operator=(Camera&& other) = default;
    ~Camera() = default;
    
    void set_background(Color&& background) { _init_pars.background = std::move(background); }
    void set_pixel_format(SDL_PixelFormat format) { _pixel_format = format; }
    std::vector<uint32_t> render_row(uint32_t j, const std::vector<Sphere>& objects);
}; // class Camera
#endif