#include "texture.h"

Color CheckerTexture::value(double u, double v, const Vec3& p) {
    auto x_int = int(std::floor(inv_scale * p.x()));
    auto y_int = int(std::floor(inv_scale * p.y()));
    auto z_int = int(std::floor(inv_scale * p.z()));

    bool is_even = (x_int + y_int + z_int) % 2 == 0;

    return is_even ? even->value(u,v,p) : odd->value(u,v,p);
}

