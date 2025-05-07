#include <cmath>
#include <algorithm>
#include <format>

#include "grid.h"

bool Cell::hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec, Grid& g) const {
    HitRecord temp_rec;
    bool hit_anything{ false };
    float closest_so_far{ ray_t.max() };
    for (const auto& tri : _triangles) {
        g._logger->add_ray_tri_int();
        if (tri.hit(r_in, Interval{ ray_t.min(), closest_so_far}, temp_rec) && temp_rec.get_t() < closest_so_far) {
            g._logger->add_true_ray_tri_int();
            hit_anything = true;
            closest_so_far = temp_rec.get_t();
            hitrec = temp_rec;
            temp_rec = HitRecord();
        }
    }
    
    return hit_anything;
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          

bool Grid::_dda(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) {
    /**
     * @brief: digital differential analyser algorithm to compute
     * ray path and intersections through the grid
     */
    Vec3f ray_dir{ r_in.direction() };
    Vec3f ray_inv_dir{ r_in.inv_dir() };
    Vec3f ray_origin{ r_in.origin() };
    float t[3]{};
    float dt[3]{};
    int step[3]{};
    int exit[3]{};

    // compute ray path through the grid
    for (uint32_t i = 0; i < 3; ++i) {
        float ray_start_cell = ((ray_origin[i] + ray_dir[i] * hitrec.get_t()) - _bbox.bounds()[0][i]);
        _cell_index[i] = std::clamp<uint32_t>(std::floor(ray_start_cell / _cellsize[i]), 0, _n[i] - 1);
        if (ray_dir[i] < 0) {
            dt[i] = -_cellsize[i] * ray_inv_dir[i];
            t[i] = hitrec.get_t() + (_cell_index[i] * _cellsize[i] - ray_start_cell) * ray_inv_dir[i];
            exit[i] = -1;
            step[i] = -1;
        } else {
            dt[i] = _cellsize[i] * ray_inv_dir[i];
            t[i] = hitrec.get_t() + ((_cell_index[i] + 1) * _cellsize[i] - ray_start_cell) * ray_inv_dir[i];
            exit[i] = static_cast<int32_t>(_n[i]);
            step[i] = 1;
        }
    }

    // check if the ray hits a triangle in the cells traversed by the ray
    bool hit{ false };
    float closest_so_far{ ray_t.max() };
    while (true) {
        uint32_t cell_idx{ std::clamp<uint32_t>(_cell_index[0] + _cell_index[1] * _n[0] + _cell_index[2] * _n[0] * _n[1], 0, _cells.size() - 1) };
        hit = _cells[cell_idx].hit(r_in, ray_t, hitrec, *this);
        auto min_idx = static_cast<uint32_t>(std::distance(t, std::min_element(t, t + 3)));
        if (hit && hitrec.get_t() < t[min_idx]) {
            break;
        }

        _cell_index[min_idx] += step[min_idx];
        if (_cell_index[min_idx] == exit[min_idx]) {
            break;
        }
        
        t[min_idx] += dt[min_idx];
    }

    return hit;
} 

void Grid::_insert_triangles() {
    for (const auto& tri : _triangles) {

        // convert to cells coordinates
        Vec3f min{};
        Vec3f max{};
        for (uint32_t i = 0; i < 3; ++i) {
            min[i] = std::floor((tri.get_bbox().bounds()[0][i] - _bbox.bounds()[0][i]) / _cellsize[i]);
            max[i] = std::floor((tri.get_bbox().bounds()[1][i] - _bbox.bounds()[0][i]) / _cellsize[i]);
        }

        auto x_min = std::clamp<uint32_t>(min.x(), 0, _n[0] - 1);
        auto x_max = std::clamp<uint32_t>(max.x(), 0, _n[0] - 1);
        auto y_min = std::clamp<uint32_t>(min.y(), 0, _n[1] - 1);
        auto y_max = std::clamp<uint32_t>(max.y(), 0, _n[1] - 1);
        auto z_min = std::clamp<uint32_t>(min.z(), 0, _n[2] - 1);
        auto z_max = std::clamp<uint32_t>(max.z(), 0, _n[2] - 1);

        for (uint32_t z = z_min; z <= z_max; ++z) {
            for (uint32_t y = y_min; y <= y_max; ++y) {
                for (uint32_t x = x_min; x <= x_max; ++x) {
                    uint32_t idx = x + y * _n[0] + z * _n[0] * _n[1];
                    _cells[idx].push_triangle(tri);
                }
            }
        }
    }
}

Grid::Grid(const BoundingBox& bbox, const std::vector<Triangle> tris, std::shared_ptr<Logger> logger, float lambda) 
: _bbox(bbox), _triangles(tris), _logger(logger), _lambda(lambda) 
{
    // heuristic grid resolution proposed in
    // https://www.researchgate.net/publication/220183660_Ray_Tracing_Animated_Scenes_Using_Coherent_Grid_Traversal
    float cbrt{ std::cbrt(_lambda * _triangles.size() / _bbox.volume()) };

    auto nx = static_cast<uint32_t>(std::floor(_bbox.size_x() * cbrt));
    auto ny = static_cast<uint32_t>(std::floor(_bbox.size_y() * cbrt));
    auto nz = static_cast<uint32_t>(std::floor(_bbox.size_z() * cbrt));

    _n[0] = nx >= 1 ? nx : 1;
    _n[1] = ny >= 1 ? ny : 1;
    _n[2] = nz >= 1 ? nz : 1;
    _cells.resize(_n[0] * _n[1] * _n[2]);
    _cellsize = Vec3f(1.f/cbrt);
    _logger->add_grid_and_cells(_n[0], _n[1], _n[2]);

    _insert_triangles();
}

bool Grid::hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) {
    if (!_bbox.hit(r_in, ray_t, hitrec)) {
        return false;
    }

    return _dda(r_in, ray_t, hitrec);
}