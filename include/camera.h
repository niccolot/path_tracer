#ifndef CAMERA_H
#define CAMERA_H

#include "SDL3/SDL.h"

#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "hittable.h"
#include "matrix.h"

typedef struct InitParams {
    uint32_t img_width;
    uint32_t img_height;
    uint32_t window_width;
    uint32_t window_height;
    uint32_t depth;
    uint32_t samples_per_pixel;
    float vfov; // vertical aperture
    float focus_dist; // distance from camera to image plane
    Vec3f lookfrom;
    Vec3f lookat;
    Color background;
    std::string outfile_name;
} init_params_t;

typedef struct CameraAngles {
    float roll = 0.f;
    float tilt = 0.f;
    float pan = 0.f;
    float theta = 90.f;
    float phi = 0.f;
} camera_angles_t;

typedef struct GeometryParams {
    std::string obj_file;
    float alpha = 0.f;
    float beta = 0.f;
    float gamma = 0.f;
    float scale = 1.f;
    Vec3f t = Vec3f();
} geometry_params_t;

class Camera {
private:
    bool _gamma_corr = true;
    Vec3f _u, _v, _w; // orthonormal basis
    Vec3f _pixel_delta_u, _pixel_delta_v; // image plane span vectors
    Vec3f _pixel00_loc; // coordinate of top-left pixel 
    Vec3f _camera_center;
    Mat4 _transformation;
    Mat4 _transformation_inv;
    SDL_PixelFormat _pixel_format;
    init_params_t _init_pars;
    camera_angles_t _angles;
    geometry_params_t _geometry_params;
    uint32_t _samples_pp_sqrt; // sqrt of samples_per_pixel
    float _samples_pp_sqrt_inv;
    float _sampling_scale;
    MeshList _meshes;

    void _move();
    void _rotate_frame();
    void _pan_tilt_roll(Vec3f& v);
    Ray _get_ray(uint32_t i, uint32_t j, uint32_t si, uint32_t sj) const;
    Vec3f _sample_square_stratified(uint32_t si, uint32_t sj) const;
    Color _trace(const Ray& r, uint32_t depth) const;
    void _write_color(Color& color, std::vector<uint32_t>& row_colors) const;
    void _gamma_correction(Color& color) const; 
    bool _hit(const MeshList& meshes, const Ray& r_in, const Interval& ray_t, HitRecord& rec) const;
    
public:
    Camera() = default;
    Camera(const init_params_t& init_pars, const camera_angles_t& angles, const geometry_params_t& g_pars);
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    Camera(Camera&&) = default;
    Camera& operator=(Camera&& other) = default;
    ~Camera() = default;
    
    void set_meshes(const objl::Loader& loader);
    void set_pixel_format(SDL_PixelFormat format) { _pixel_format = format; }
    std::vector<uint32_t> render_row(uint32_t j) const;
}; // class Camera
#endif