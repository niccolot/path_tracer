#ifndef UTILS_H
#define UTILS_H

#include <numbers>
#include <string>
#include <filesystem>
#include <format>

#include "vec3.h"

namespace Utils {
inline float degs_to_rads(float d) {
    return d * std::numbers::pi / 180.f;
}

inline std::string strip_extenstions(const std::string& filename) {
    auto dot_pos = filename.find('.');
    if (dot_pos != std::string::npos) {
        return filename.substr(0, dot_pos);
    }

    return filename;
}

inline void set_directory(const std::string& path) {
    if (!std::filesystem::exists(path)) {
        if (!std::filesystem::create_directories(path)) {
            throw std::runtime_error{ std::format("Failed to create '{}' direcotry\n", path) };
        }
    }
}

inline void set_pmin_pmax(Vec3f& p_min, Vec3f& p_max, const Vec3f& v) {
    p_min.set_x(std::min(p_min.x(), v.x()));
    p_min.set_y(std::min(p_min.y(), v.y()));
    p_min.set_z(std::min(p_min.z(), v.z()));

    p_max.set_x(std::max(p_max.x(), v.x()));
    p_max.set_y(std::max(p_max.y(), v.y()));
    p_max.set_z(std::max(p_max.z(), v.z()));
}
} // namespace Utils



#endif