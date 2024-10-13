#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include "hittable.h"

using Color = Vec3;

void write_color(std::ostream& out, const Color& pixel_color);
Color ray_color(const Ray& r, const Hittable& world);
#endif