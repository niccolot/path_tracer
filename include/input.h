#ifndef INPUT_H
#define INPUT_H

#include <string>

#include <nlohmann/json.hpp>

#include "app.h"
#include "camera.h"

using njson = nlohmann::json;

void from_json(const njson& j, Vec3f& v);
void from_json(const njson& j, init_params_t& p);
void from_json(const njson& j, camera_angles_t& angles);
void from_json(const njson& j, geometry_params_t& g);
init_params_t init_from_json(const std::string& datapath);
camera_angles_t angles_from_json(const std::string& datapath);
geometry_params_t geometry_from_json(const std::string& datapath);
#endif