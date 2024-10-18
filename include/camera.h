#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "hittable.h"
#include "utils.h"

using Color = Vec3;

class Camera {
    private:
        // image and camera
        int img_height_val;
        int img_width_val;
        double aspect_ratio_val;
        Vec3 lookfrom; // where the camera is looking from
        Vec3 lookat; // point that the camera is looking at
        double vfov; // vertical field of view
        int samples_per_pixel;
        double pixel_samples_scale;
        int max_depth; // max number of ray bounces
        Vec3 vup = Vec3(0, 1, 0); // camera-relative 'up' direction
        Vec3 u, v, w; // camera frame basis vectors

        //viewport
        Vec3 center;
        Vec3 pixel00_loc;
        Vec3 pixel_delta_u;
        Vec3 pixel_delta_v;

        void initialize();

        void write_color(std::ostream& out, const Color& pixel_color);
        Color ray_color(const Ray& r, int depth, const Hittable& world);
        Ray get_ray(int i, int j) const;
    
    public:
        Camera(int width, 
            double asp_rat, 
            Vec3&& lookfrom,
            Vec3&& lookat,
            double vfov = 90,
            int samples = 100, 
            int depth = 50);

        // constructor with standard parameters for quick demos
        Camera() : Camera(
            512,
            16/9,
            Vec3(0,0,1),
            Vec3(0,0,0.99),
            45,
            1,
            10
        ) {}

        void render(const Hittable& world);
        void set_aspect_ratio(double aspect_ratio) { aspect_ratio_val = aspect_ratio; }
        void set_image_width(int img_width) { img_width_val = img_width; }
        void set_samples_per_pixel(int samples) { samples_per_pixel = samples; }
        void set_lookfrom(const Vec3& lfrom) { lookfrom = lfrom; }
        void set_lookat(const Vec3& lat) { lookat = lat; }
        void set_lookfrom(Vec3&& lfrom) { lookfrom = std::move(lfrom); }
        void set_lookat(Vec3&& lat) { lookat = std::move(lat); }
}; // class Camera
#endif