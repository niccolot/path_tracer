#include <algorithm>

#include "mesh.h"

Mesh::Mesh(
        int nfaces, 
        const std::vector<int>& fi, 
        const std::vector<int>& vi, 
        const std::vector<Vec3>& p) : 
            
            _nfaces(nfaces),
            _vertex_index(vi),
            _face_index(fi),
            _P(p) {
        /**
         * @brief takes as input the polygon's data and instanciate
         * a triangulated mesh of it
         * @param fi array with an element for each face and the 
         * value of each element corresponds to the number of vertices the 
         * face is composed of
         * @param vi array with vertices indexes for each face
         * @param p array with the 3d positions of each vertex
         * @details if e.g. n = fi[0] and m = fi[1]..., the first n elements of the vi array 
         * are the indices of the vertices composing the first face, the next m elements
         * are the indices of the vertices composing the second face and so on
         */
    
    // in the polygon arrays, especially when loaded from 
    // a file, sometimes there are more vertices than needed so 
    // it is better to explicitly calculate these values
    _vert_arr_size = std::reduce(fi.begin(), fi.end());
    _max_vert_idx = *std::max_element(vi.begin(), vi.end()); 
}