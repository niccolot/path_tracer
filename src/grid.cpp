#include <cmath>
#include <algorithm>

#include "grid.h"

bool Cell::hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) const {
    std::cout << "tris in this cell: " << _triangles.size() << "\n";
    HitRecord temp_rec;
    bool hit_anything{ false };
    float closest_so_far{ ray_t.max() };
    for (const auto& tri : _triangles) {
        if (tri.hit(r_in, temp_rec) && temp_rec.get_t() < closest_so_far) {
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
    Vec3f gridMin_rayO{ r_in.origin() - _bbox.bounds()[0] }; // vector from grid min point to ray origin
    Vec3f cell_rayO{ gridMin_rayO / _cellsize }; // position of origin cell in terms of the number of cells

    // compute ray path through the grid
    for (uint32_t i = 0; i < 3; ++i) {
        if (ray_dir[i] < 0) {
            _dt[i] = -(_bbox.bounds()[1] - _bbox.bounds()[0])[i] * ray_inv_dir[i];
            _t[i] = ((std::floor(gridMin_rayO[i])) * _cellsize[i] - gridMin_rayO[i]) * ray_inv_dir[i];
        } else {
            _dt[i] = (_bbox.bounds()[1] - _bbox.bounds()[0])[i] * ray_inv_dir[i];
            _t[i] = ((std::floor(cell_rayO[i]) + 1) * _cellsize[i] - gridMin_rayO[i]) * ray_inv_dir[i];
        }
    }

    float t{};
    auto min_idx{ std::distance(_t, std::min_element(_t, _t + 3)) };
    bool hit{ false };

    // get first hitted cell 
    for (uint32_t i = 0; i < 3; ++i) {
        float id = (r_in.origin()[i] + ray_dir[i] * hitrec.get_t() - _bbox.bounds()[0][i]) / _n[i];
        _cell_index[i] = static_cast<uint32_t>(id);
    }

    // check if the ray hits a triangle in the cells traversed by the ray
    while(true) {
        uint32_t idx{ _cell_index[0] + _cell_index[1] * _n[0] + _cell_index[2] * _n[0] * _n[1] };
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
    /**
     * @brief: checks if a ray exited the grid. Since
     * the indexes are uint32_t, if the ray exits backwards (negative indices),
     * the check is still working since the index will overflow
     */
    bool out_of_bounds_x{ _cell_index[0] > _n[0] };
    bool out_of_bounds_y{ _cell_index[1] > _n[1] };
    bool out_of_bounds_z{ _cell_index[2] > _n[2] };

    return out_of_bounds_x || out_of_bounds_y || out_of_bounds_z;
}

void Grid::_insert_triangles() {
    for (const auto& tri : _triangles) {
        // convert to cells coordinates
        Vec3f min = (tri.get_bbox().bounds()[0] - _bbox.bounds()[0]) / _cellsize;
        Vec3f max = (tri.get_bbox().bounds()[1] - _bbox.bounds()[0]) / _cellsize;

        auto x_min = std::clamp<uint32_t>(std::floor(min.x()), 0, _n[0] - 1);
        auto x_max = std::clamp<uint32_t>(std::floor(max.x()), 0, _n[0] - 1);
        auto y_min = std::clamp<uint32_t>(std::floor(min.y()), 0, _n[1] - 1);
        auto y_max = std::clamp<uint32_t>(std::floor(max.y()), 0, _n[1] - 1);
        auto z_min = std::clamp<uint32_t>(std::floor(min.z()), 0, _n[2] - 1);
        auto z_max = std::clamp<uint32_t>(std::floor(max.z()), 0, _n[2] - 1);

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

Grid::Grid(const BoundingBox& bbox, const std::vector<Triangle> tris, float l) 
: _bbox(bbox), _triangles(tris), _lambda(l) 
{
    float inv_vol{ 1.f / _bbox.volume() };

    // heuristic grid resolution proposed in
    // https://www.researchgate.net/publication/220183660_Ray_Tracing_Animated_Scenes_Using_Coherent_Grid_Traversal
    float cbrt{ std::cbrt(_lambda * _triangles.size() * inv_vol) };

    _n[0] = static_cast<uint32_t>(_bbox.size_x() * cbrt);
    _n[1] = static_cast<uint32_t>(_bbox.size_y() * cbrt);
    _n[2] = static_cast<uint32_t>(_bbox.size_z() * cbrt);
    _cells.resize(_n[0] * _n[1] * _n[2]);
    _cellsize = Vec3f(_bbox.size_x() / _n[0], _bbox.size_y() / _n[1], _bbox.size_z() / _n[2]);

    _insert_triangles();
    std::cout << "num cells: " << _cells.size() << "\n";
    std::cout << "cellsize: " << _cellsize << "\n";
    std::cout << "grid volume: " << _cellsize[0] * _cellsize[1] * _cellsize[2] * _cells.size() << "\n";
}

bool Grid::hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) {
    if (!_bbox.hit(r_in, ray_t, hitrec)) {
        return false;
    }

    return _dda(r_in, ray_t, hitrec);
}