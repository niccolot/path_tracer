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

void from_json(const njson& j, camera_angles_t& angles) {
    if (j.count("roll") != 0) {
        j.at("roll").get_to(angles.roll);
    }
    if (j.count("tilt") != 0) {
        j.at("tilt").get_to(angles.tilt);
    }
    if (j.count("pan") != 0) {
        j.at("pan").get_to(angles.pan);
    }
    if (j.count("theta") != 0) {
        j.at("theta").get_to(angles.theta);
    }
    if (j.count("phi") != 0) {
        j.at("phi").get_to(angles.phi);
    }
}

void from_json(const njson& j, geometry_params_t& g) {
    j.at("obj_file").get_to(g.obj_file);
    if (j.count("alpha") != 0) {
        j.at("alpha").get_to(g.alpha);
    }
    if (j.count("beta") != 0) {
        j.at("beta").get_to(g.beta);
    }
    if (j.count("gamma") != 0) {
        j.at("gamma").get_to(g.gamma);
    }
    if (j.count("scale") != 0) {
        j.at("scale").get_to(g.scale);
    }
    if (j.count("t") != 0) {
        j.at("t").get_to(g.t);
    }
}

init_params_t init_from_json(const std::string& datapath) {
    std::ifstream file(datapath);
    if (!file) {
        throw std::runtime_error{ std::format("Invalid input: file '{}' does not exists", datapath) };
    }

    njson j;
    file >> j;
    lowercase_keys(j);
    file.close();

    return j.get<init_params_t>();
}

camera_angles_t angles_from_json(const std::string& datapath) {
    std::ifstream file(datapath);
    if (!file) {
        std::clog << std::format("Camera angles file '{}' not found, setting angles to default values\n", datapath);
        return camera_angles_t{};
    }

    njson j;
    file >> j;
    lowercase_keys(j);
    file.close();

    return j.get<camera_angles_t>();
}

geometry_params_t get_geometry(njson& j) {
    lowercase_keys(j);

    return j.get<geometry_params_t>();
}

std::vector<geometry_params_t> geometries_from_json(const std::string& datapath) {
    std::ifstream file(datapath);
    if (!file) {
        throw std::runtime_error{ std::format("Invalid input: file '{}' does not exists", datapath) };
    }

    njson j = njson::parse(file);
    auto& geometries = j["geometries"];
    std::vector<geometry_params_t> g_vec;
    g_vec.reserve(geometries.size());
    for (auto& g : geometries) {
        g_vec.push_back(std::move(get_geometry(g)));
    }

    return g_vec;
}