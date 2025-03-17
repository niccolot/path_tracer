#include <format>
#include <fstream>

#include "input.h"

void from_json(const njson& j, Vec3f& v) {
    if (!(j.is_array() && j.size() == 3)) {
        throw std::runtime_error("Invalid Vec3f format in json file");
    }

    v.set_x(j[0].get<float>());
    v.set_y(j[1].get<float>());
    v.set_z(j[2].get<float>());
}

void from_json(const njson& j, init_params_t& p) {
    j.at("img_width").get_to(p.img_width);
    j.at("img_height").get_to(p.img_height);
    j.at("window_width").get_to(p.window_width);
    j.at("window_height").get_to(p.window_height);
    j.at("lookfrom").get_to(p.lookfrom);
    j.at("lookat").get_to(p.lookat);
    j.at("background").get_to(p.background);
    j.at("vfov").get_to(p.vfov);
    j.at("focus_dist").get_to(p.focus_dist);
}

init_params_t init_from_json(const std::string& datapath) {
    std::ifstream file(datapath);
    if (!file) {
        throw std::runtime_error{ std::format("Invalid input: file {} does not exists", datapath) };
    }

    njson j;
    file >> j;

    return j.get<init_params_t>();
}