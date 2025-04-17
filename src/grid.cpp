#include <cmath>

#include "grid.h"

Grid::Grid(const BoundingBox& bbox, float l) : _bbox(bbox), _lambda(l) {
    float inv_vol = 1.f / _bbox.volume();
    _nx = uint32_t(_bbox.size_x() * std::cbrt(_lambda * _bbox.num_tris() * inv_vol));
    _ny = uint32_t(_bbox.size_y() * std::cbrt(_lambda * _bbox.num_tris() * inv_vol));
    _nz = uint32_t(_bbox.size_z() * std::cbrt(_lambda * _bbox.num_tris() * inv_vol)); 
}