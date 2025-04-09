#ifndef MAT3_H
#define MAT3_H

#include <array>

#include "vec3.h"

using Mat3 = std::array<std::array<float, 3>, 3>;

inline Vec3f mat_vec_prod(const Mat3& m, const Vec3f& v) {
    float x{};
    float y{};
    float z{};

    for (uint32_t i = 0; i < 3; ++i) {
        x += v[i]*m[0][i];
        y += v[i]*m[1][i];
        z += v[i]*m[2][i];
    }

    return Vec3f(x,y,z);
}

inline void mat_vec_prod_inplace(const Mat3& m, Vec3f& v) {
    float x{};
    float y{};
    float z{};

    for (uint32_t i = 0; i < 3; ++i) {
        x += v[i]*m[0][i];
        y += v[i]*m[1][i];
        z += v[i]*m[2][i];
    }

    v.set_x(x);
    v.set_y(y);
    v.set_z(z);
}

Mat3 rotation_3d(float t, const Vec3f& n);
Mat3 roll_rotation(float t);
Mat3 tilt_rotation(float t);
Mat3 pan_rotation(float t);
#endif