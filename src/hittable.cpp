#include <memory>
#include <cassert>

#include "hittable.h"

bool Sphere::hit(const Ray& r_in, const Interval& ray_t, HitRecord& hitrec) const {
    Vec3 oc = _center - r_in.origin(); // vector from ray origin to sphere center

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
: _v0(v0), _v1(v1), _v2(v2), _face_normal(n), _color(std::move(col))
{
    _v0v1 = _v1 - _v0;
    _v0v2 = _v2 - _v0;
}

bool Triangle::hit(const Ray& r_in, [[maybe_unused]] const Interval& ray_t, HitRecord& hitrec) const {
    /**
     * @brief: ray-triangle intersection method using moller-trumbore algorithm
     */
    constexpr float tol = 1e-8;

    Vec3f p_vec = cross(r_in.direction(), _v0v2);
    float det = dot(_v0v1, p_vec);
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
    Vec3f n = unit_vector(cross(_v0v1, _v0v2));
    hitrec.set_normal(get_face_normal());
    hitrec.set_color(_color * std::fabs(dot(hitrec.get_normal(), r_in.direction())));

    return true;
}

Mesh::Mesh(const objl::Mesh& mesh) {
    _vertices = std::move(mesh.Vertices);
    _indices = std::move(mesh.Indices);
    float r = mesh.MeshMaterial.Ka.X;
    float g = mesh.MeshMaterial.Ka.Y;
    float b = mesh.MeshMaterial.Ka.Z;
    _color = Color(r,g,b);
    uint32_t num_vertices = _vertices.size();
    uint32_t num_indices = _indices.size();
    
    assert(_vertices.size() % 3 == 0);

    _triangles.reserve(num_vertices / 3);
    for (uint32_t i = 0; i <_indices.size(); i += 3) {
        auto v0 = _vertices[_indices[i]];
        auto v0_pos = v0.Position;
        auto v0_normal = v0.Normal;
        auto v1 = _vertices[_indices[i + 1]];
        auto v1_pos = v1.Position;
        auto v1_normal = v1.Normal;
        auto v2 = _vertices[_indices[i + 2]];
        auto v2_pos = v2.Position;
        auto v2_normal = v2.Normal;
        Triangle tri = Triangle(
            Vec3f(v0_pos.X, v0_pos.Y, v0_pos.Z), 
            Vec3f(v1_pos.X, v1_pos.Y, v1_pos.Z),
            Vec3f(v2_pos.X, v2_pos.Y, v2_pos.Z),
            _color);       

        _triangles.emplace_back(tri);
    }
}

bool Mesh::hit(const Ray& r_in, [[maybe_unused]] const Interval& ray_t, HitRecord& hitrec) const {
    HitRecord temp_rec;
    bool hit_anything = false;
    float closest_so_far = ray_t.max();
    for (const auto& tri : _triangles) {
        if (tri.hit(r_in, Interval(ray_t.min(), closest_so_far), temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.get_t();
            hitrec = std::move(temp_rec);
        }
    }

    return hit_anything;
}

MeshList::MeshList(const objl::Loader& loader) {
    for (const auto& mesh : loader.LoadedMeshes) {
        Mesh m(mesh);
        _mesh_list.emplace_back(m);
    }
}

bool MeshList::hit(const Ray& r_in, [[maybe_unused]] const Interval& ray_t, HitRecord& hitrec) const {
    HitRecord temp_rec;
    bool hit_anything = false;
    float closest_so_far = ray_t.max();
    for (const auto& mesh : _mesh_list) {
        if (mesh.hit(r_in, Interval(ray_t.min(), closest_so_far), temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.get_t();
            hitrec = std::move(temp_rec);
        }
    }

    return hit_anything;
}