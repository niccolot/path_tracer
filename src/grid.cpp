#include <cmath>
#include <algorithm>

#include "grid.h"

bool Cell::hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) const {
    HitRecord temp_rec;
    bool hit_anything{ false };
    float closest_so_far{ ray_t.max() };
    for (const auto& tri : _triangles) {
        if (tri.hit(r_in, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.get_t();
            hitrec = std::move(temp_rec);
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
    Vec3f gridMin_rayO{ r_in.origin() - _bbox.bounds()[0] }; // vector from grid min point to ray origin
    Vec3f cell_rayO{ gridMin_rayO / _cellsize }; // position of origin cell in terms of the number of cells

    // compute ray path through the grid
    for (uint32_t i = 0; i < 3; ++i) {
        if (ray_dir[i] < 0) {
            _dt[i] = -(_bbox.bounds()[1] - _bbox.bounds()[0])[i] / ray_dir[i];
            _t[i] = ((std::floor(gridMin_rayO[i])) * _cellsize[i] - gridMin_rayO[i]) / ray_dir[i];
        } else {
            _dt[i] = (_bbox.bounds()[1] - _bbox.bounds()[0])[i] / ray_dir[i];
            _t[i] = ((std::floor(cell_rayO[i]) + 1) * _cellsize[i] - gridMin_rayO[i]) / ray_dir[i];
        }
    }

    float t{};
    auto min_idx{ std::distance(_t, std::min_element(_t, _t + 3)) };
    bool hit{ false };

    // check if the ray hits a triangle in the cells traversed by the ray
    while(true) {
        _cell_index[0] = static_cast<uint32_t>(r_in.origin().x());
        _cell_index[1] = static_cast<uint32_t>(r_in.origin().y());
        _cell_index[2] = static_cast<uint32_t>(r_in.origin().z());
        uint32_t idx{ _cell_index[0] + _cell_index[1] + _nx + _cell_index[2] * _nx * _ny };
        hit = _cells[idx].hit(r_in, ray_t, hitrec);
        t = _t[min_idx];
        _t[min_idx] += _dt[min_idx];
        if (ray_dir[min_idx] < 0) {
            --_cell_index[min_idx];
        } else {
            ++_cell_index[min_idx];
        }

        if (hit && hitrec.get_t() < t) {
            return hit;
        }

        if (_check_boundary()) {
            return hit;
        }
    }
} 

bool Grid::_check_boundary() const {
    bool out_of_bounds_x{ _cell_index[0] < 0 || _cell_index[0] > _nx };
    bool out_of_bounds_y{ _cell_index[1] < 0 || _cell_index[1] > _ny };
    bool out_of_bounds_z{ _cell_index[2] < 0 || _cell_index[2] > _nz };

    return out_of_bounds_x || out_of_bounds_y || out_of_bounds_z;
}

Grid::Grid(const BoundingBox& bbox, const std::vector<Triangle> tris, float l = 3.5) 
: _bbox(bbox), _triangles(tris), _lambda(l) 
{
    float inv_vol{ 1.f / _bbox.volume() };

    // heuristic grid resolution proposed in
    // https://www.researchgate.net/publication/220183660_Ray_Tracing_Animated_Scenes_Using_Coherent_Grid_Traversal
    float cbrt{ std::cbrt(_lambda * _triangles.size() * inv_vol) };

    float inv_cbrt{ 1.f / cbrt };
    _nx = static_cast<uint32_t>(_bbox.size_x() * cbrt);
    _ny = static_cast<uint32_t>(_bbox.size_y() * cbrt);
    _nz = static_cast<uint32_t>(_bbox.size_z() * cbrt);
    _cells.reserve(_nx * _ny * _nz);
    _cellsize.set_x(inv_cbrt);
    _cellsize.set_y(inv_cbrt);
    _cellsize.set_z(inv_cbrt);
}

bool Grid::hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) {
    if (!_bbox.hit(r_in, ray_t, hitrec)) {
        return false;
    }

    return _dda(r_in, ray_t, hitrec);
}