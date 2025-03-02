#ifndef CAMERA_H
#define CAMERA_H

#include <condition_variable>
#include <mutex>
#include <queue>

#include "vec3.h"
#include "color.h"
#include "hittable.h"
#include "utils.h"
#include "multithreading.h"

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
        std::vector<Vec3> image;

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
        int depth_cutoff; // for limiting the amount of splitted refrected/reflected rays
        Vec3 vup; // camera-relative 'up' direction
        Vec3 u, v, w; // camera frame basis vectors
        Vec3 defocus_disk_u;
        Vec3 defocus_disk_v;
        Color background;
        int samples_sqrt; // square root of samples per pixel
        double inv_samples_sqrt;

        //viewport
        Vec3 center;
        Vec3 pixel00_loc;
        Vec3 pixel_delta_u;
        Vec3 pixel_delta_v;

        std::vector<job_block_t> image_blocks;
        std::mutex mutex;
        std::condition_variable cv;

        Color ray_color(const Ray& r, int depth, const Hittable& world, const Hittable& lights) const;
        Ray get_ray(int i, int j, int s_i, int s_j) const;
        Vec3 defocus_disk_sample() const;
        Vec3 sample_square_stratified(int s_i, int s_j) const;
        void color_per_job(const Hittable& world, const Hittable& lights, job_block_t& job);
        void reconstruct_image(std::ostream& out);
    
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

        void render(const Hittable& world, const Hittable& lights);
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