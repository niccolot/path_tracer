#ifndef CAMERA_H
#define CAMERA_H

#include <cstdint>

#include "vec3.h"
#include "color.h"
#include "ray.h"

class Camera {

private:
    bool _gamma_correction;
    uint32_t _img_width, _img_height;
    float _aspect_ratio;
    Vec3f _lookfrom, _lookat;
    Color _background;
    float _vfov; // vertical aperture
    float _focus_dist; // distance from camera to image plane
    Vec3f _vup; // up direction in camera frame of reference
    Vec3f _u, _v, _w; // orthonormal basis
    Vec3f _pixel_delta_u, _pixel_delta_v; // image plane span vectors
    Vec3f _pixel00_loc; // coordinate of top-left pixel 

    Ray get_ray(uint32_t i, uint32_t j);
    Color ray_color(const Ray& r);
    void write_color(Color& color);
    void gamma_correction(Color& color);    
    
public:
    Camera(
        uint32_t width,
        float aspect_ratio,
        Vec3f lookfrom = Vec3f(),
        Vec3f lookat = Vec3f(0.f, 0.f, -1.f),
        Color background = Color(),
        float vfov = 90.f,
        float focus_dist = 10.f
    );

    void set_background(const Color& background) { _background = background; }
    void render();
}; // class Camera

#endif