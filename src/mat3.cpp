#include <cmath>

#include "mat3.h"

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

Mat3 roll_rotation(float t) {
    Mat3 m{
        std::array{ std::cos(t), -std::sin(t), 0.f },
        std::array{ std::sin(t), std::cos(t), 0.f },
        std::array{ 0.f, 0.f, 1.f }
    };

    return m;
}

Mat3 tilt_rotation(float t) {
    Mat3 m{
        std::array{ 1.f, 0.f, 0.f},
        std::array{ 0.f, std::cos(t), -std::sin(t) },
        std::array{ 0.f, std::sin(t), std::cos(t) }
    };

    return m;
}

Mat3 pan_rotation(float t) {
    Mat3 m{
        std::array{ std::cos(t), 0.f, std::sin(t) },
        std::array{ 0.f, 1.f, 0.f },
        std::array{ -std::sin(t), 0.f, std::cos(t) }
    };

    return m;
}
