#ifndef UTILS_H
#define UTILS_H

#include <numbers>
#include <string>
#include <filesystem>
#include <format>

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
} // namespace Utils



#endif