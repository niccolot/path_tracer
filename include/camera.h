#ifndef CAMERA_H
#define CAMERA_H

#include <memory>

#include "SDL3/SDL.h"

#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "mesh.h"
#include "matrix.h"
#include "input.h"
#include "logger.h"

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
    std::vector<geometry_params_t> _geometries;
    std::shared_ptr<Logger> _logger;
    uint32_t _samples_pp_sqrt; // sqrt of samples_per_pixel
    float _samples_pp_sqrt_inv;
    float _sampling_scale;
    MeshList _meshes;

    void _move();
    void _rotate_frame();
    Ray _get_ray(uint32_t i, uint32_t j, uint32_t si, uint32_t sj) const;
    Vec3f _sample_square_stratified(uint32_t si, uint32_t sj) const;
    Color _trace(const Ray& r, uint32_t depth) const;
    void _write_color(Color& color, std::vector<uint32_t>& row_colors) const;
    void _gamma_correction(Color& color) const; 
    
public:
    Camera() = default;
    Camera(
        const init_params_t& init_pars, 
        const camera_angles_t& angles, 
        const std::vector<geometry_params_t>& geometries,
        std::shared_ptr<Logger> logger
    );
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    Camera(Camera&&) = default;
    Camera& operator=(Camera&& other) = default;
    ~Camera() = default;
    
    void set_meshes();
    void set_pixel_format(SDL_PixelFormat format) { _pixel_format = format; }
    std::vector<uint32_t> render_row(uint32_t j);
}; // class Camera
#endif