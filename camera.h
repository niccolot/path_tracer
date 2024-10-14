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

        Vec3 center;
        Vec3 pixel00_loc;
        Vec3 pixel_delta_u;
        Vec3 pixel_delta_v;

        void initialize();
        void write_color(std::ostream& out, const Color& pixel_color);
        Color ray_color(const Ray& r, const Hittable& world);
    
    public:
        void render(const Hittable& world);
        void set_aspect_ratio(double aspect_ratio) { aspect_ratio_val = aspect_ratio; }
        void set_image_width(int img_width) { img_width_val = img_width; }
}; // class Camera
#endif