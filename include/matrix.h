#ifndef MATRIX_H
#define MATRIX_H

#include <array>

#include "vec3.h"

using Mat3 = std::array<std::array<float, 3>, 3>;
using Mat4 = std::array<std::array<float, 4>, 4>;

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

inline void mat4_vec3_prod_inplace(const Mat4& m, Vec3f& v) {
    float x{};
    float y{};
    float z{};

    for (uint32_t i = 0; i < 3; ++i) {
        x += v[i]*(m[0][i] + m[3][0]);
        y += v[i]*(m[1][i] + m[3][1]);
        z += v[i]*(m[2][i] + m[3][2]);
    }

    v.set_x(x);
    v.set_y(y);
    v.set_z(z);
}

Mat3 rotation_3d(float t, const Vec3f& n);
Mat3 frame_rotation(float a, float b, float c);
Mat3 transpose(const Mat3& m);
Mat4 frame_transformation(float a, float b, float c, float s, const Vec3f& t);
Mat4 frame_transformation_inv(float a, float b, float c, float s, const Vec3f& t);
#endif