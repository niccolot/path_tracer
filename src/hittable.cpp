#include <memory>
#include <cassert>
#include <algorithm>

#include "hittable.h"
#include "utils.h"

bool Sphere::hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) const {
    Vec3f oc = _center - r_in.origin(); // vector from ray origin to sphere center

    // a = r_in.direction().length_squared(), here assumed already unitary
    // for the Ray class constructor
    const float a = 1; 
    float h = dot(r_in.direction(), oc); // h = -b/2 in 2nd order eq roots formula
    float c = oc.length_squared() - _radius * _radius;

    float delta = h * h - a * c;
    if (delta < 0) {
        return false;
    }

    float delta_sqrt = std::sqrt(delta);
    float root = (h - delta_sqrt) / a;
    if (!ray_t.surrounds(root)) {
        root = (h + delta_sqrt) / a;
        if (!ray_t.surrounds(root)) {
            return false;
        }
    }

    hitrec.set_t(root);
    hitrec.set_hit_point(r_in.at(root));

    // by spheres property dividing by radius avoid sqrt normalization
    Vec3f normal = (hitrec.get_hit_point() - _center) / _radius;
    hitrec.set_normal(normal, r_in.direction());
    hitrec.set_color(_color * std::fabs(dot(hitrec.get_normal(), r_in.direction())));

    return true;
}

Triangle::Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const Color&col) {
    _v0 = v0;
    _v1 = v1;
    _v2 = v2;
    _v0v1 = v1.pos - v0.pos;
    _v0v2 = v2.pos - v0.pos;
    _face_normal = unit_vector(_v0.normal + _v1.normal + _v2.normal);
    _color = col;
}

bool Triangle::hit(const Ray& r_in, HitRecord& hitrec) const {
    /**
     * @brief: ray-triangle intersection method using moller-trumbore algorithm
     */
    const float tol = 1e-8;

    Vec3f p_vec = cross(r_in.direction(), _v0v2);
    float det = dot(_v0v1, p_vec);

    // use std::fabs(det) < tol to disable backface culling
    if (det < tol) {
        return false;
    }

    float det_inv = 1.f / det;
    Vec3f t_vec = r_in.origin() - _v0.pos;
    float u = dot(t_vec, p_vec) * det_inv;
    if (u < 0 || u > 1) {
        return false;
    }

    Vec3f q_vec = cross(t_vec, _v0v1);
    float v = dot(r_in.direction(), q_vec) * det_inv;
    if (v < 0 || u + v > 1) {
        return false;
    }
    
    float t = dot(_v0v2, q_vec) * det_inv;
    hitrec.set_u(u);
    hitrec.set_v(v);
    hitrec.set_t(t);
    hitrec.set_hit_point(r_in.at(t));
    hitrec.set_normal(unit_vector((1.f - u - v) * _v0.normal + u * _v1.normal + v * _v2.normal));
    hitrec.set_color(_color * std::max(0.f, -dot(hitrec.get_normal(), r_in.direction())));


    return true;
}

Mesh::Mesh(const objl::Mesh& mesh, Mat4&& m, Mat4&& m_inv) {
    _vertices = mesh.Vertices;
    _indices = mesh.Indices;
    _transf = std::move(m);
    _transf_inv = std::move(m_inv);
    float r = mesh.MeshMaterial.Ka.X;
    float g = mesh.MeshMaterial.Ka.Y;
    float b = mesh.MeshMaterial.Ka.Z;
    _color = Color(r,g,b);
    
    assert(_vertices.size() % 3 == 0);

    _triangles.reserve(_vertices.size() / 3);
    for (uint32_t i = 0; i <_indices.size(); i += 3) {
        auto v0 = _vertices[_indices[i]];
        auto v0_pos = Vec3f(v0.Position);
        auto v0_normal = Vec3f(v0.Normal);
        auto v1 = _vertices[_indices[i + 1]];
        auto v1_pos = Vec3f(v1.Position);
        auto v1_normal = Vec3f(v1.Normal);
        auto v2 = _vertices[_indices[i + 2]];
        auto v2_pos = Vec3f(v2.Position);
        auto v2_normal = Vec3f(v2.Normal);
        
        mat4_vec3_prod_inplace(_transf, v0_pos);
        mat4_vec3_prod_inplace(_transf, v1_pos);
        mat4_vec3_prod_inplace(_transf, v2_pos);
        mat4_vec3_prod_inplace(_transf_inv, v0_normal);
        mat4_vec3_prod_inplace(_transf_inv, v1_normal);
        mat4_vec3_prod_inplace(_transf_inv, v2_normal);

        _triangles.emplace_back(
            Vertex{v0_pos, v0_normal}, 
            Vertex{v1_pos, v1_normal}, 
            Vertex{v2_pos, v2_normal}, 
            _color
        );
    }
}

void MeshList::add(const objl::Loader& loader, const geometry_params_t& g) {
    for (const auto& mesh : loader.LoadedMeshes) {
        Mat4 transformation = frame_transformation(
            degs_to_rads(g.alpha),
            degs_to_rads(g.beta),
            degs_to_rads(g.gamma),
            g.scale,
            g.t);

        Mat4 transformation_inv = frame_transformation_inv(
            degs_to_rads(g.alpha),
            degs_to_rads(g.beta),
            degs_to_rads(g.gamma),
            g.scale,
            g.t);

        Mesh m(mesh, std::move(transformation), std::move(transformation_inv));
        _triangles.reserve(m.get_triangles().size());
        for (const auto& tri : m.get_triangles()) {
            _triangles.push_back(std::move(tri));
        }
    }
}

bool MeshList::hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) const {
    HitRecord temp_rec;
    bool hit_anything = false;
    float closest_so_far = ray_t.max();
    for (const auto& tri : _triangles) {
        if (tri.hit(r_in, temp_rec) && temp_rec.get_t() < closest_so_far) {
            hit_anything = true;
            closest_so_far = temp_rec.get_t();
            hitrec = std::move(temp_rec);
            temp_rec = HitRecord();
        }
    }

    return hit_anything;
}