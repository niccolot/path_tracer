#include <cmath>
#include <algorithm>

#include "grid.h"

bool Cell::hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec, Grid& g) const {
    HitRecord temp_rec;
    bool hit_anything{ false };
    float closest_so_far{ ray_t.max() };
    for (const auto& tri : _triangles) {
        g._logger->add_ray_tri_int();
        if (tri.hit(r_in, temp_rec) && temp_rec.get_t() < closest_so_far) {
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
    Vec3f Ocell_Oray{ (ray_origin + ray_dir * hitrec.get_t()) - _bbox.bounds()[0] };
    uint32_t cell[3];
    // compute ray path through the grid
    for (uint32_t i = 0; i < 3; ++i) {
        cell[i] = std::clamp<uint32_t>(Ocell_Oray[i] / _cellsize[i], 0, _n[i] - 1);
        if (ray_dir[i] < 0) {
            _dt[i] = -_cellsize[i] * ray_inv_dir[i];
            _t[i] = hitrec.get_t() + (cell[i] * _cellsize[i] - Ocell_Oray[i]) * ray_inv_dir[i];
        } else {;
            _dt[i] = _cellsize[i] * ray_inv_dir[i];
            _t[i] = hitrec.get_t() + ((cell[i] + 1) * _cellsize[i] - Ocell_Oray[i]) * ray_inv_dir[i];
        }
    }

    // get first hitted cell 
    for (uint32_t i = 0; i < 3; ++i) {
        float id = (r_in.origin()[i] + ray_dir[i] * hitrec.get_t() - _bbox.bounds()[0][i]) / _cellsize[i];
        auto idx = std::clamp<uint32_t>(id, 0, _n[i] - 1);
        _cell_index[i] = idx;
    }

    // check if the ray hits a triangle in the cells traversed by the ray
    float t{};
    bool hit{ false };
    while(!_check_boundary()) {
        auto min_idx{ std::distance(_t, std::min_element(_t, _t + 3)) };
        uint32_t cell_idx{ std::clamp<uint32_t>(_cell_index[0] + _cell_index[1] * _n[0] + _cell_index[2] * _n[0] * _n[1], 0, _cells.size() - 1) };

        hit = _cells[cell_idx].hit(r_in, ray_t, hitrec, *this);
        t = _t[min_idx];
        if (ray_dir[min_idx] < 0) {
            --_cell_index[min_idx];
        } else {
            ++_cell_index[min_idx];
        }
        
        if (hit && hitrec.get_t() < t) {
            break;
        }
        _t[min_idx] += _dt[min_idx];
    }

    return hit;
} 

bool Grid::_check_boundary() const {
    /**
     * @brief: checks if a ray exited the grid. Since
     * the indexes are uint32_t, if the ray exits backwards (negative indices),
     * the check is still working since the index will overflow
     */
    bool out_of_bounds_x{ _cell_index[0] >= _n[0] };
    bool out_of_bounds_y{ _cell_index[1] >= _n[1] };
    bool out_of_bounds_z{ _cell_index[2] >= _n[2] };

    return out_of_bounds_x || out_of_bounds_y || out_of_bounds_z;
}

void Grid::_insert_triangles() {
    for (const auto& tri : _triangles) {
        // convert to cells coordinates
        Vec3f min = (tri.get_bbox().bounds()[0] - _bbox.bounds()[0]) / _cellsize;
        Vec3f max = (tri.get_bbox().bounds()[1] - _bbox.bounds()[0]) / _cellsize;

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

    _n[0] = static_cast<uint32_t>(_bbox.size_x() * cbrt);
    _n[1] = static_cast<uint32_t>(_bbox.size_y() * cbrt);
    _n[2] = static_cast<uint32_t>(_bbox.size_z() * cbrt);
    _cells.resize(_n[0] * _n[1] * _n[2]);
    _cellsize = Vec3f(1.f/cbrt);

    _insert_triangles();
}

bool Grid::hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) {
    if (!_bbox.hit(r_in, ray_t, hitrec)) {
        return false;
    }
    
    return _dda(r_in, ray_t, hitrec);
}