#ifndef INPUT_H
#define INPUT_H

#include <string>

#include <nlohmann/json.hpp>

#include "app.h"

using njson = nlohmann::json;

void from_json(const njson& j, Vec3f& v);
void from_json(const njson& j, init_params_t& p);
init_params_t init_from_json(const std::string& datapath);
#endif