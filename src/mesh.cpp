#include <memory>
#include <cassert>
#include <algorithm>

#include "mesh.h"
#include "utils.h"

Mesh::Mesh(const objl::Mesh& mesh, Mat4&& m, Mat4&& m_inv) {
    _bounds.resize(2);
    _bounds[0] = Vec3f(inf, inf, inf);
    _bounds[1] = Vec3f(-inf, -inf, -inf);

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

        _set_min_max(v0_pos);
        _set_min_max(v1_pos);
        _set_min_max(v2_pos);
        
        _triangles.emplace_back(
            Vertex{v0_pos, v0_normal}, 
            Vertex{v1_pos, v1_normal}, 
            Vertex{v2_pos, v2_normal}, 
            _color
        );
    }
}

void Mesh::_set_min_max(const Vec3f& v) {
    _bounds[0].set_x(std::min(_bounds[0].x(), v.x()));
    _bounds[0].set_y(std::min(_bounds[0].y(), v.y()));
    _bounds[0].set_z(std::min(_bounds[0].z(), v.z()));

    _bounds[1].set_x(std::max(_bounds[1].x(), v.x()));
    _bounds[1].set_y(std::max(_bounds[1].y(), v.y()));
    _bounds[1].set_z(std::max(_bounds[1].z(), v.z()));
}

bool Mesh::hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) const {
    /**
     * @brief: ray-mesh bounding box intersection algorithm
     */
    float t_min = (_bounds[r_in.sign_x()].x() - r_in.origin().x()) * r_in.inv_dir().x();
    float t_max = (_bounds[1 - r_in.sign_x()].x() - r_in.origin().x()) * r_in.inv_dir().x();
    float ty_min = (_bounds[r_in.sign_y()].y() - r_in.origin().y()) * r_in.inv_dir().y();
    float ty_max = (_bounds[1 - r_in.sign_y()].y() - r_in.origin().y()) * r_in.inv_dir().y();

    if ((t_min > ty_max) || (ty_min > t_max)) {
        return false;
    }

    t_min = t_min > ty_min ? t_min : ty_min;
    t_max = t_max < ty_max ? t_max : ty_max;

    float tz_min = (_bounds[r_in.sign_z()].z() - r_in.origin().z()) * r_in.inv_dir().z();
    float tz_max = (_bounds[1 - r_in.sign_z()].z() - r_in.origin().z()) * r_in.inv_dir().z();

    if ((t_min > tz_max) || (tz_min > t_max)) {
        return false;
    }

    t_min = t_min > tz_min ? t_min : tz_min;
    t_max = t_max < tz_max ? t_max : tz_max;

    t_min = t_min > 0 ? t_min : t_max;
    if (t_min < 0) {
            return false;
    }

    if (!ray_t.contains(t_min)) {
        return false;
    }

    hitrec.set_t(t_min); 

    return true;
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
        Mesh m(mesh, std::move(transformation), std::move(transformation_inv));
        _logger->add_tris(m.get_triangles().size());
        _meshes.push_back(std::move(m));
    }
}

bool MeshList::hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) const {
    HitRecord temp_rec;
    bool hit_anything = false;
    float closest_so_far = ray_t.max();
    for (const auto& mesh : _meshes) {
        if (mesh.hit(
                r_in, 
                Interval(ray_t.min(), closest_so_far),
                temp_rec)) 
        {
            for (const auto& tri : mesh.get_triangles()) {
                _logger->add_ray_tri_int();
                if (tri.hit(r_in, temp_rec) && temp_rec.get_t() < closest_so_far) {
                    _logger->add_true_ray_tri_int();
                    hit_anything = true;
                    closest_so_far = temp_rec.get_t();
                    hitrec = std::move(temp_rec);
                    temp_rec = HitRecord();
                }
            }
            
        }
    }

    return hit_anything;
}