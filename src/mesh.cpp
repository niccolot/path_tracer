#include <algorithm>

#include "mesh.h"
#include "triangle.h"

Mesh::Mesh(
        int nfaces, 
        const std::vector<int>& fi, 
        const std::vector<int>& vi, 
        const std::vector<Vec3>& p,
        const std::vector<Vec3>& normals,
        std::shared_ptr<Material> mat) : 
            
            _nfaces(nfaces),
            _face_index(fi),
            _vertex_index(vi),
            _P(p),
            _normals(normals),
            _mat(mat) {
        /**
         * @brief takes as input the polygon's data and instanciate
         * a triangulated mesh of it
         * @param fi array with an element for each face and the 
         * value of each element corresponds to the number of vertices the 
         * face is composed of
         * @param vi array with vertices indexes for each face
         * @param p array with the 3d positions of each vertex
         * @param normals arrays with each vertex normal from the polygon
         * @details if e.g. n = fi[0] and m = fi[1]..., the first n elements of the vi array 
         * are the indices of the vertices composing the first face, the next m elements
         * are the indices of the vertices composing the second face and so on
         */
    
    _max_vert_idx = *std::max_element(vi.begin(), vi.end()) + 1;
    
    _ntris = _max_vert_idx - 2;
    _tris_index = std::vector<int>(_ntris * 3);
    
    int l{};
    for (int i=0, k=0; i<_nfaces; ++i) { // for each face
        for (int j=0; j<_face_index[i] - 2; ++j) { // for each triangle in the face
            _tris_index[l] = _vertex_index[k];
            _tris_index[l+1] = _vertex_index[k+j+1];
            _tris_index[l+2] = _vertex_index[k+j+2];
            l += 3;
        }
        k += _face_index[i];
    }

    auto range = _tris_index.size();
    for (size_t i=0; i<range; i+=3) {
        auto v0 = _P[_tris_index[i]];
        auto v1 = _P[_tris_index[i+1]];
        auto v2 = _P[_tris_index[i+2]];
        Color col = Vec3::random();
        auto random_mat = std::make_shared<Lambertian>(col); // to be deleted just before merge
        auto triangle = std::make_shared<Triangle>(v0, v1, v2, random_mat, true);
        _triangles.push_back(triangle);
        _bbox = AxisAlignedBBox(_bbox, triangle->bounding_box());
    }
}