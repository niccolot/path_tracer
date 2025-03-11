#include <cmath>

#include "camera.h"
#include "utils.h"

Camera::Camera(
    uint32_t width,
    float aspect_ratio,
    Vec3f lookfrom,
    Vec3f lookat,
    Color background,
    float vfov,
    float focus_dist
) :
    _img_width(width),
    _aspect_ratio(aspect_ratio),
    _lookfrom(lookfrom),
    _lookat(lookat),
    _background(background),
    _vfov(vfov),
    _focus_dist(focus_dist)
{
    _img_height = uint32_t(_img_width / _aspect_ratio);
    _img_height = (_img_height > 1) ? _img_height : 1; // img_height at leas 1 pixel
    _vup = Vec3f(0,1.f,0);

    // antiparallel to view direction
    _w = unit_vector(_lookfrom - _lookat);
    
    // perpendicular to view direction and _vup
    _u = cross(_vup, _w);

    // camera up direction in the global frame of reference
    _v = cross(_w, _u);

    // image plane
    float h = std::tan(0.5f * degs_to_rads(_vfov));
    float img_plane_height = 2.f * h * _focus_dist;
    float img_plane_width = img_plane_height * float(_img_width) / _img_height;
    Vec3f img_plane_u = img_plane_width * _u;
    Vec3 img_plane_v = -img_plane_height * _v;
    _pixel_delta_u = img_plane_u / img_plane_width;
    _pixel_delta_v = img_plane_v / img_plane_height;
    Vec3f img_plane_upper_left = _lookfrom - 
                                    _focus_dist * _w -
                                    0.5f * (img_plane_u + img_plane_v);
    
    _pixel00_loc = img_plane_upper_left + 0.5f * (_pixel_delta_u + _pixel_delta_v);
}

void Camera::render() {

}