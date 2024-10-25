#ifndef CAMERA_H
#define CAMERA_H

#include "vec3.h"
#include "color.h"
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

        // in photography terms where
        // focal length = distance from camera center to 
        // image plane and focus distance = distance from camera 
        // center to the plane where everything is in focus, here we
        // set focal lenght = focus distance by placing the camera
        // pixel grid (viewport) right in from of the focus plane
        double focus_dist;
        double defocus_angle; // variation angle of rays from lookfrom though pixels
        int samples_per_pixel; // number of pixel resampling for antialiasing
        double pixel_samples_scale;
        int max_depth; // max number of ray bounces
        Vec3 vup; // camera-relative 'up' direction
        Vec3 u, v, w; // camera frame basis vectors
        Vec3 defocus_disk_u;
        Vec3 defocus_disk_v;
        Color background;

        //viewport
        Vec3 center;
        Vec3 pixel00_loc;
        Vec3 pixel_delta_u;
        Vec3 pixel_delta_v;

        void write_color(std::ostream& out, const Color& pixel_color);
        Color ray_color(const Ray& r, int depth, const Hittable& world);
        Ray get_ray(int i, int j) const;

        //friend inline Vec3 defocus_disk_sample();
        inline Vec3 defocus_disk_sample() const {
            /**
             * @brief returns a random point in the defocus disk
             */

            auto p = random_in_unit_disk();
            return center + (p.x() * defocus_disk_u) + (p.y() * defocus_disk_v);
        }
    
    public:
        Camera(int width, 
            double asp_rat, 
            Vec3&& lookfrom,
            Vec3&& lookat,
            double vfov = 90,
            double focus_dist = 10,
            double defocus_angle = 0,
            int samples = 100, 
            int depth = 50);

        // constructor with standard parameters for quick demos
        Camera() : Camera(
            512,
            16./9.,
            Vec3(0,0,1),
            Vec3(0,0,0.99),
            45,
            10,
            0,
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
        void set_background(const Color& bg) { background = bg; }
        void set_background(Color&& bg) { background = std::move(bg); }
}; // class Camera
#endif