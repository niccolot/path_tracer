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

Triangle::Triangle(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, const Vec3f& n, const Color& col) 
: _v0(v0), _v1(v1), _v2(v2), _face_normal(n), _color(col)
{
    _v0v1 = _v1 - _v0;
    _v0v2 = _v2 - _v0;
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
    Vec3f t_vec = r_in.origin() - _v0;
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
    hitrec.set_t(t);
    hitrec.set_hit_point(r_in.at(t));
    hitrec.set_normal(get_face_normal());
    hitrec.set_color(_color * std::fabs(dot(hitrec.get_normal(), r_in.direction())));

    return true;
}

Mesh::Mesh(const objl::Mesh& mesh, const Mat4& m, const Mat4& m_inv) {
    _vertices = std::move(mesh.Vertices);
    _indices = std::move(mesh.Indices);
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
        Vec3f n = unit_vector((v0_normal + v1_normal + v2_normal) / 3.f);
        
        mat4_vec3_prod_inplace(m, v0_pos);
        mat4_vec3_prod_inplace(m, v1_pos);
        mat4_vec3_prod_inplace(m, v2_pos);
        mat4_vec3_prod_inplace(m_inv, n);

        Triangle tri = Triangle(v0_pos, v1_pos, v2_pos, n, _color);       

        _triangles.emplace_back(std::move(tri));
    }
}

void MeshList::add(const objl::Loader& loader, const geometry_params_t& g) {
    for (const auto& mesh : loader.LoadedMeshes) {
        const auto transformation = frame_transformation(
            degs_to_rads(g.alpha),
            degs_to_rads(g.beta),
            degs_to_rads(g.gamma),
            g.scale,
            g.t);

        const auto transformation_inv = frame_transformation_inv(
            degs_to_rads(g.alpha),
            degs_to_rads(g.beta),
            degs_to_rads(g.gamma),
            g.scale,
            g.t);

        Mesh m(mesh, transformation, transformation_inv);
        _triangles.reserve(m.get_triangles().size());
        for (const auto& tri : m.get_triangles()) {
            _triangles.emplace_back(std::move(tri));
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
        }
    }

    return hit_anything;
}