#include <cmath>

#include "matrix.h"

Mat3 rotation_3d(float t, const Vec3f& n) {
    /**
     * @brief: rotation matrix of angle t around (assumed unitary)
     * axis n
     */
    float nxnx = n.x() * n.x();
    float nyny = n.y() * n.y();
    float nznz = n.z() * n.z();

    float nxny = n.x() * n.y();
    float nxnz = n.x() * n.z();
    float nynz = n.y() * n.z();

    float cost = std::cos(t);
    float sint = std::sin(t);

    Mat3 m{
        std::array{ cost + nxnx * (1 - cost), nxny * (1 - cost) - n.z() * sint, nxnz * (1 - cost) + n.y() * sint },
        std::array{ nxny * (1 - cost) + n.z() * sint, cost + nyny * (1 - cost), nynz * (1 - cost) - n.x() * sint },
        std::array{ nxnz * (1 - cost) - n.y() * sint, nynz * (1 - cost) + n.x() * sint, cost + nznz * (1 - cost) }
    };

    return m;
}

Mat3 frame_rotation(float a, float b, float c) {
    /**
     * @brief: frame of reference rotation matrix
     * @details: this rotation is implemented as a composition of x_rot, y_rot, z_rot
     * in this order
     * @param a: angle around x axis
     * @param b: angle around y axis
     * @param c: angle around z axis
     */
    float sin_a = std::sin(a);
    float cos_a = std::cos(a);
    float sin_b = std::sin(b);
    float cos_b = std::cos(b);
    float sin_c = std::sin(c);
    float cos_c = std::cos(c);

    Mat3 m{
        std::array{ cos_b * cos_c, sin_a * sin_b * cos_c - cos_a * sin_c, cos_a * sin_b * cos_c + sin_a * sin_c },
        std::array{ cos_b * sin_c, sin_a * sin_b * sin_c + cos_a * cos_c, cos_a * sin_b * sin_c - sin_a * cos_c },
        std::array{ -sin_b, sin_a * cos_b, cos_a * cos_b }    
    };

    return m;
}

Mat3 transpose(const Mat3& m) {
    Mat3 t{
        std::array{ m[0][0], m[1][0], m[2][0] },
        std::array{ m[0][1], m[1][1], m[2][1] },
        std::array{ m[0][2], m[1][2], m[2][2] }
    };

    return t;
}

Mat4 frame_transformation(float a, float b, float c, float s, const Vec3f& t) {
    /**
     * @brief: 4x4 matrix encompassing a frame rotation, a scaling factor
     * and a frame translation
     * @param s: scale factor
     * @param t: translation vector
     */

    Mat3 rot = frame_rotation(a,b,c);
    Mat4 m{
        std::array{ s * rot[0][0], s * rot[0][1], s * rot[0][2], 0.0f },
        std::array{ s * rot[1][0], s * rot[1][1], s * rot[1][2], 0.0f },
        std::array{ s * rot[2][0], s * rot[2][1], s * rot[2][2], 0.0f },
        std::array{ t.x(), t.y(), t.z(), 1.f }
    };

    return m;
}

Mat4 frame_transformation_inv(float a, float b, float c, float s, const Vec3f& t) {
    /**
     * @brief: "inverse" frame transformation, to be used for normals, the 
     * transposition of the rotation matrix is done manually inplace
     * @param s: the direct scale factor, to be inverted later
     */
    Mat3 rot = frame_rotation(a,b,c);
    float s_inv = 1.f/s;
    Mat4 m{
        std::array{ s_inv * rot[0][0], s_inv * rot[1][0], s_inv * rot[2][0], 0.0f },
        std::array{ s_inv * rot[0][1], s_inv * rot[1][1], s_inv * rot[2][1], 0.0f },
        std::array{ s_inv * rot[0][2], s_inv * rot[1][2], s_inv * rot[2][2], 0.0f },
        std::array{ -t.x(), -t.y(), -t.z(), 1.f }
    };

    return m;
}
