#ifndef MESH_H
#define MESH_H

#include <vector>

#include "triangle.h"

class Mesh : public HittableList {
    private:
        int _nfaces; // num faces of initial polygon
        std::vector<int> _face_index;
        std::vector<int> _vertex_index;
        std::vector<Vec3> _P; // positions of each vertex
        std::vector<Vec3> _normals;
        std::shared_ptr<Material> _mat;

        // vector of size 3*_ntris that stores the 3 vertices for each triangle in cc ordering
        std::vector<int> _tris_index;

        std::vector<std::shared_ptr<Hittable>> _triangles;
        AxisAlignedBBox _bbox;
        int _max_vert_idx;
        int _ntris; // number of triangles in mesh

    public:
        Mesh(
            int nfaces, 
            const std::vector<int>& fi, 
            const std::vector<int>& vi, 
            const std::vector<Vec3>& p,
            const std::vector<Vec3>& normals,
            std::shared_ptr<Material> mat);

        const AxisAlignedBBox& bounding_box() const override { return _bbox; };
        std::vector<std::shared_ptr<Hittable>>& get_objects() { return _triangles; }
}; // class Mesh
#endif