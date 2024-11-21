#ifndef MESH_H
#define MESH_H

#include <vector>

#include "hittable.h"

class Mesh : public HittableList {
    private:
        std::vector<int> _face_index;
        std::vector<int> _vertex_index;
        std::vector<Vec3> _P; // positions of each vertex
        int _nfaces; // num faces of initial polygon
        int _vert_arr_size;
        int _max_vert_idx;

    public:
        Mesh(
            int nfaces, 
            const std::vector<int>& fi, 
            const std::vector<int>& vi, 
            const std::vector<Vec3>& p);

        bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override;
}; // class Mesh
#endif