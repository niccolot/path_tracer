#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include <cstdint>

using Color = Vec3f;

class Camera {
private:
    uint32_t _img_width, _img_height;
    float _aspect_ratio;
    float _vfov; // vertical aperture
    float _focus_dist; // distance from camera to image plane
    Vec3f _lookfrom, _lookat;
    Vec3f _vup; // up direction in camera frame of reference
    Vec3f _u, _v, _w; // orthonormal basis
    Vec3f _pixel_delta_u, _pixel_delta_v; // image plane span vectors
    Vec3f _pixel00_loc; // coordinate of top-left pixel 
    Color _background;
    
public:
    Camera(
        uint32_t width,
        float aspect_ratio,
        Vec3f lookfrom = Vec3f(),
        Vec3f lookat = Vec3f(0,0,-1.f),
        Color background,
        float vfov = 90.f,
        float focus_dist = 10.f
    );

    void render();
}; // class Camera

#endif