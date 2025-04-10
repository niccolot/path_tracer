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

Triangle::Triangle(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, const Color& col) : _v0(v0), _v1(v1), _v2(v2), _color(col) {
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
    if (std::fabs(det) < tol) {
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
    hitrec.set_normal(n, r_in.direction());
    hitrec.set_color(_color * std::fabs(dot(hitrec.get_normal(), r_in.direction())));

    return true;
}

Mesh::Mesh(const objl::Mesh& mesh) {
    _vertices = std::move(mesh.Vertices);
    _indices = std::move(mesh.Indices);

    std::ofstream file("temp.txt");
    file << "Verices:\n";
    for (uint32_t j = 0; j < _vertices.size(); ++j) {
        file << "V" << j << ": " <<
        "P(" << _vertices[j].Position.X << ", " << _vertices[j].Position.Y << ", " << _vertices[j].Position.Z << ") " <<
        "N(" << _vertices[j].Normal.X << ", " << _vertices[j].Normal.Y << ", " << _vertices[j].Normal.Z << ") " <<
        "TC(" << _vertices[j].TextureCoordinate.X << ", " << _vertices[j].TextureCoordinate.Y << ")\n"; 
    }

    file << "Indices:\n";
    for (uint32_t j = 0; j < _indices.size(); j += 3) {
        file << "T" << j / 3 << ": " << _indices[j] << ", " << _indices[j + 1] << ", " << _indices[j + 2] << "\n";
    }
}