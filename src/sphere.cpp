#include <cmath>
#include "sphere.h"

Sphere::Sphere(
    const Vec3& static_center, 
    double radius, 
    std::shared_ptr<Material> mat) : 
    
    center(static_center, Vec3()), 
    radius(std::fmax(0., radius)), 
    mat(mat) {
    
    // stationary sphere
    auto rvec = Vec3(radius, radius, radius);
    bbox = AxisAlignedBBox(static_center - rvec, static_center + rvec);
}

Sphere::Sphere(
    const Vec3& center1,
    const Vec3& center2, 
    double radius, 
    std::shared_ptr<Material> mat) :

    center(center1, center2 - center1),
    radius(std::fmax(0., radius)),
    mat(mat) {
    
    // moving sphere
    auto rvec = Vec3(radius, radius, radius);
    AxisAlignedBBox box1(center.at(0) - rvec, center.at(0) + rvec);
    AxisAlignedBBox box2(center.at(1) - rvec, center.at(1) + rvec);

    bbox = AxisAlignedBBox(box1, box2);
}

bool Sphere::hit(const Ray& r, Interval ray_t, HitRecord& rec) const {
    Vec3 current_center = center.at(r.time());
    Vec3 oc = current_center - r.origin();
    auto a = r.direction().length_squared();  
    auto h = dot(r.direction(), oc); // h = -b/2 in 2nd order equation roots formula
    auto c = oc.length_squared() - radius*radius;

    auto discriminant = h*h - a*c;
    if (discriminant < 0) {
        return false;
    }

    auto sqrt_delta = std::sqrt(discriminant);

    // find the nearest root that lies in the acceptable range [ray_tmin, ray_tmax]
    auto root = (h - sqrt_delta) / a;
    if (!ray_t.surrounds(root)) {
        root = (h + sqrt_delta) / a;
        if(!ray_t.surrounds(root)) {
            return false;
        }
    }

    rec.set_t(root);
    rec.set_point(r.at(root));
    
    Vec3 outward_normal = (rec.point() - current_center) / radius;
    rec.set_face_normal(r, outward_normal);
    get_sphere_uv(outward_normal, rec.u(), rec.v());
    rec.set_material(mat);

    return true;
}

double Sphere::pdf_value(const Vec3& origin, const Vec3& direction) const {
    /**
     * @brief sampling for just the exposed part of the sphere
     * @details this method only works for stationary spheres
     */
    HitRecord rec;
    if (!this->hit(Ray(origin, direction), Interval(0.001, infinity), rec)) {
        return 0;
    }

    double dist_squared = (center.at(0) - origin).length_squared();
    double cos_theta_max = std::sqrt(1 - radius*radius/dist_squared);
    double solid_angle = 2*pi*(1 - cos_theta_max);

    if (std::isnan(solid_angle) || std::fabs(solid_angle) < 1e-3) {
        return 0;
    } else {
        return 1 / solid_angle;
    }
}

Vec3 Sphere::random(const Vec3& origin) const {
    Vec3 direction = center.at(0) - origin;
    double distance_squared = direction.length_squared();
    ONB uvw(direction);
    
    return uvw.transform(random_to_sphere(radius, distance_squared));
}

void Sphere::get_sphere_uv(const Vec3& p, double& u, double& v) {
    /**
     * @brief given a point p on the sphere it returns
     * the (u,v) coordinates for texture mapping
     * @details p: a given point on the sphere of radius one, centered at the origin.
     * u: returned value [0,1] of angle around the Y axis from X=-1.
     * v: returned value [0,1] of angle from Y=-1 to Y=+1.
     * <1 0 0> yields <0.50 0.50>   <-1  0  0> yields <0.00 0.50>
     * <0 1 0> yields <0.50 1.00>   < 0 -1  0> yields <0.50 0.00>
     * <0 0 1> yields <0.25 0.50>   < 0  0 -1> yields <0.75 0.50>
     */

    auto theta = std::acos(-p.y());
    auto phi = std::atan2(-p.z(), p.x()) + pi;

    u = phi / (2*pi);
    v = theta / pi;
}

Vec3 Sphere::random_to_sphere(double radius, double distance_squared) {
    auto r1 = RandomUtils::random_double();
    auto r2 = RandomUtils::random_double();
    auto z = 1 + r2*(std::sqrt(1 - radius*radius/distance_squared) - 1);

    auto phi = 2*pi*r1;
    auto x = std::cos(phi) * std::sqrt(1 - z*z);
    auto y = std::sin(phi) * std::sqrt(1 - z*z);

    return Vec3(x,y,z);
}