#include <memory>
#include <cassert>
#include <algorithm>

#include "mesh.h"
#include "utils.h"

Mesh::Mesh(const objl::Mesh& mesh, Mat4&& m, Mat4&& m_inv, MeshList& list) {
    Vec3f pmin{ inf };
    Vec3f pmax{ -inf };

    _transf = std::move(m);
    _transf_inv = std::move(m_inv);
    float r = mesh.MeshMaterial.Ka.X;
    float g = mesh.MeshMaterial.Ka.Y;
    float b = mesh.MeshMaterial.Ka.Z;
    _color = Color(r,g,b);
    
    assert(mesh.Vertices.size() % 3 == 0);

    _triangles.reserve(mesh.Vertices.size() / 3);
    for (uint32_t i = 0; i < mesh.Indices.size(); i += 3) {
        auto v0 = mesh.Vertices[mesh.Indices[i]];
        auto v0_pos = Vec3f(v0.Position);
        auto v0_normal = Vec3f(v0.Normal);
        auto v1 = mesh.Vertices[mesh.Indices[i + 1]];
        auto v1_pos = Vec3f(v1.Position);
        auto v1_normal = Vec3f(v1.Normal);
        auto v2 = mesh.Vertices[mesh.Indices[i + 2]];
        auto v2_pos = Vec3f(v2.Position);
        auto v2_normal = Vec3f(v2.Normal);
        
        mat4_vec3_prod_inplace(_transf, v0_pos);
        mat4_vec3_prod_inplace(_transf, v1_pos);
        mat4_vec3_prod_inplace(_transf, v2_pos);
        mat4_vec3_prod_inplace(_transf_inv, v0_normal);
        mat4_vec3_prod_inplace(_transf_inv, v1_normal);
        mat4_vec3_prod_inplace(_transf_inv, v2_normal);

        Utils::set_pmin_pmax(pmin, pmax, v0_pos);
        Utils::set_pmin_pmax(pmin, pmax, v1_pos);
        Utils::set_pmin_pmax(pmin, pmax, v2_pos);
        
        _triangles.emplace_back(
            Vertex{v0_pos, v0_normal}, 
            Vertex{v1_pos, v1_normal}, 
            Vertex{v2_pos, v2_normal}, 
            _color
        );
    }

    _grid = Grid{ BoundingBox(pmin, pmax), _triangles, list._logger };
}

bool Mesh::hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) const {

    return _grid.hit(r_in, ray_t, hitrec);
}

void MeshList::add(const objl::Loader& loader, const geometry_params_t& g) {
    for (const auto& mesh : loader.LoadedMeshes) {
        Mat4 transformation = frame_transformation(
            Utils::degs_to_rads(g.alpha),
            Utils::degs_to_rads(g.beta),
            Utils::degs_to_rads(g.gamma),
            g.scale,
            g.t);

        Mat4 transformation_inv = frame_transformation_inv(
            Utils::degs_to_rads(g.alpha),
            Utils::degs_to_rads(g.beta),
            Utils::degs_to_rads(g.gamma),
            g.scale,
            g.t);

        _logger->add_mesh_obj();
        Mesh m(mesh, std::move(transformation), std::move(transformation_inv), *this);
        _logger->add_tris(m.get_triangles().size());
        _meshes.push_back(std::move(m));
    }
}

bool MeshList::hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) const {
    HitRecord temp_rec;
    bool hit_anything{ false };
    float closest_so_far{ ray_t.max() };
    for (const auto& mesh : _meshes) {
        if (mesh.hit(r_in, Interval(ray_t.min(), closest_so_far), temp_rec) && 
            temp_rec.get_t() < closest_so_far) 
        {
            hit_anything = true;
            closest_so_far = temp_rec.get_t();
            hitrec = temp_rec;
            temp_rec = HitRecord();
        }
    }

    return hit_anything;
}