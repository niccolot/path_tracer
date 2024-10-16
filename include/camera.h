#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "hittable.h"

using Color = Vec3;

class Camera {
    private:
        int img_height_val;
        int img_width_val;
        double aspect_ratio_val;
        int samples_per_pixel;
        double pixel_samples_scale;
        int max_depth; // max number of ray bounces

        Vec3 center;
        Vec3 pixel00_loc;
        Vec3 pixel_delta_u;
        Vec3 pixel_delta_v;

        void initialize();

        void write_color(std::ostream& out, const Color& pixel_color);
        Color ray_color(const Ray& r, int depth, const Hittable& world);
        Ray get_ray(int i, int j) const;
    
    public:
        Camera(int samples = 1, int depth = 10) : 
            samples_per_pixel(samples),
            max_depth(depth) {}

        Camera(int width, double asp_rat, int samples = 100, int depth = 50) : 
            img_width_val(width),
            aspect_ratio_val(asp_rat),
            samples_per_pixel(samples),
            max_depth(depth) {} 

        void render(const Hittable& world);
        void set_aspect_ratio(double aspect_ratio) { aspect_ratio_val = aspect_ratio; }
        void set_image_width(int img_width) { img_width_val = img_width; }
        void set_samples_per_pixel(int samples) { samples_per_pixel = samples; }
}; // class Camera
#endif