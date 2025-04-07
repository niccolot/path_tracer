#include <format>
#include <fstream>

#include "input.h"

void to_lower(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

void lowercase_keys(njson& j) {
    if (j.is_object()) {
        njson temp;  
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string lower_key = it.key();
            to_lower(lower_key);
            temp[lower_key] = std::move(it.value());  
        }

        j = std::move(temp);  

    } else if (j.is_array()) {
        for (auto& item : j) {
            lowercase_keys(item); 
        }
    }
}

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
    if (j.count("lookfrom") != 0) {
        j.at("lookfrom").get_to(p.lookfrom);
    } else {
        p.lookfrom = Vec3f();
    }
    if (j.count("lookat") != 0) {
        j.at("lookat").get_to(p.lookat);
    } else {
        p.lookat = Vec3f(0, 0, -1);
    }
    if (j.count("background") != 0) {
        j.at("background").get_to(p.background);
    } else {
        p.background = Color();
    }
    if (j.count("vfov") != 0) {
        j.at("vfov").get_to(p.vfov);
    } else {
        p.vfov = 90;
    }
    if (j.count("focus_dist") != 0) {
        j.at("focus_dist").get_to(p.focus_dist);
    } else {
        p.focus_dist = 10;
    }
    if (j.count("vup") != 0) {
        j.at("vup").get_to(p.vup);
    } else {
        p.vup = Vec3f(0, 1, 0);
    }
    if (j.count("outfile_name") != 0) {
        j.at("outfile_name").get_to(p.outfile_name);
    } else {
        p.outfile_name = "image.png";
    }
    if (j.count("depth") != 0) {
        j.at("depth").get_to(p.depth);
    } else {
        p.depth = 10;
    }
    if (j.count("samples_per_pixel") != 0) {
        j.at("samples_per_pixel").get_to(p.samples_per_pixel);
    } else {
        p.samples_per_pixel = 10;
    }
}

init_params_t init_from_json(const std::string& datapath) {
    std::ifstream file(datapath);
    if (!file) {
        throw std::runtime_error{ std::format("Invalid input: file {} does not exists", datapath) };
    }

    njson j;
    file >> j;
    lowercase_keys(j);

    return j.get<init_params_t>();
}