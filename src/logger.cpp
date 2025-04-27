#include <format>
#include <iostream>
#include <fstream>

#include "logger.h"
#include "utils.h"

Logger::Logger(const std::string& outdir, const std::string& filename) {
    _img_file = Utils::strip_extenstions(filename) + ".png";
    _log_file = Utils::strip_extenstions(filename) + "_log.txt";
    _outdir = outdir;
}

void Logger::_print_log(std::ostream& out) const {
    out << std::format("Rendering log for image '{}'\n\n", _img_file);
    out << std::format("Total mesh objects: {}\n", _mesh_objects);
    out << std::format("Total grids: {}\n", _grids.size());
    for (uint32_t i = 0; i < _grids.size(); ++i) {
        uint32_t nx = _grids[i][0];
        uint32_t ny = _grids[i][1];
        uint32_t nz = _grids[i][2];
        out << std::format("Grid {}: nx = {}, ny = {}, nz = {}, total cells: {}\n", i, nx, ny, nz, nx * ny * nz);
    }

    out << std::format("Total triangles: {}\n", _triangles);
    out << std::format("Total Ray-Triangle intersections tested: {}\n", _total_ray_tri_intersections);
    out << std::format("Succesfull Ray-Triangle hits: {}\n", _true_ray_tri_intersections);

    auto hitrate = static_cast<float>(_true_ray_tri_intersections) / _total_ray_tri_intersections;
    out << std::format("Hit rate: {:.2f}%\n", hitrate * 100);
    out << std::format("Rendering time: {} [s]\n", _render_time);
}   

void Logger::log() const {
    _print_log(std::cout);

    std::ofstream file;
    file.open(_outdir + _log_file);
    _print_log(file);
    file.close();
}