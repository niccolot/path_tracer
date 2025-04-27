#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <cstdint>
#include <vector>

class Logger {
private:
    std::string _img_file;
    std::string _log_file;
    std::string _outdir;
    uint32_t _mesh_objects{};
    uint32_t _triangles{};
    uint32_t _total_ray_tri_intersections{};
    uint32_t _true_ray_tri_intersections{};
    float _render_time{};
    std::vector<std::vector<uint32_t>> _grids; 

    void _print_log(std::ostream& out) const;

public:
    Logger(const std::string& outdir, const std::string& filename);
    void add_mesh_obj() { ++_mesh_objects; }
    void add_tris(uint32_t tris) { _triangles += tris; }
    void add_grid_and_cells(uint32_t nx, uint32_t ny, uint32_t nz) { _grids.emplace_back(std::vector<uint32_t>{nx, ny, nz}); }
    void add_ray_tri_int() { ++_total_ray_tri_intersections; }
    void add_true_ray_tri_int() { ++_true_ray_tri_intersections; }
    void set_rendertime(float t) { _render_time = t; }
    
    void log() const;
}; // class Logger

#endif