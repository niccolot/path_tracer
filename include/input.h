#ifndef INPUT_H
#define INPUT_H

#include <string>
#include <set>

#include <nlohmann/json.hpp>

#include "vec3.h"
#include "color.h"

using njson = nlohmann::json;

typedef struct InitParams {
    uint32_t img_width;
    uint32_t img_height;
    uint32_t window_width;
    uint32_t window_height;
    uint32_t depth;
    uint32_t samples_per_pixel;
    float vfov; // vertical aperture
    float focus_dist; // distance from camera to image plane
    Vec3f lookfrom;
    Vec3f lookat;
    Color background;
    std::string outfile_name;
} init_params_t;

typedef struct CameraAngles {
    float tilt{}; // rotates camera around x
    float pan{}; // rotates camera around y
    float roll{}; // rotates camera around z
    float theta{ 90.f }; // moves camera on the YZ plane
    float phi{}; // moves camera on the XZ plane
} camera_angles_t;

typedef struct GeometryParams {
    std::string obj_file;
    float alpha{}; // rotates mesh around x
    float beta{}; // rotates mesh around y
    float gamma{}; // rotates mesh around z
    float scale{1.f};
    Vec3f t{}; // translates mesh
} geometry_params_t;

void from_json(const njson& j, Vec3f& v);
void from_json(const njson& j, init_params_t& p);
void from_json(const njson& j, camera_angles_t& angles);
void from_json(const njson& j, geometry_params_t& g);
void to_lower(std::string& str);
void lowercase_keys(njson& j);
void validate_keys(njson& j, std::set<std::string>&& allowed_keys);
init_params_t init_from_json(const std::string& datapath);
camera_angles_t angles_from_json(const std::string& datapath);
geometry_params_t get_geometry(njson& j);
std::vector<geometry_params_t> geometries_from_json(const std::string& datapath);
#endif