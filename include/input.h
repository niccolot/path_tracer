#ifndef INPUT_H
#define INPUT_H

#include <string_view>

#include <nlohmann/json.hpp>

#include "app.h"

using njson = nlohmann::json;

void from_json(const njson& j, Vec3f& v);
void from_json(const njson& j, init_params_t& p);
init_params_t init_from_json(std::string_view datapath);
#endif