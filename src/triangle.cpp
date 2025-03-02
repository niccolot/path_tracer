#include "triangle.h"
#include "utils.h"

bool Triangle::is_interior(double a, double b, HitRecord& rec) const {
    Interval unit_interval = Interval(0,1);
    
    // totally outside the planar figure
    if (!unit_interval.contains(a) || !unit_interval.contains(b)) {
        return false;
    }
    
    // too much diagonally displaced in order to be inside the triangle's shape
    if (!unit_interval.contains(1 - a - b)) {
        return false;
    }

    rec.set_u(a);
    rec.set_v(b);

    return true;
}

Vec3 Triangle::random(const Vec3& origin) const {
    double a, b;
    while (true) {
        a = RandomUtils::random_double();
        b = RandomUtils::random_double();

        if ((a + b) < 1) break;
    }

    auto p = _Q + a*_u + b*_v;

    return p - origin;
}

std::shared_ptr<HittableList> quad_pyramid(const Quad& base, double h, std::shared_ptr<Material> mat) {
    return pyramid<Quad>(base, h, mat);
}

std::shared_ptr<HittableList> triangle_pyramid(const Triangle& base, double h, std::shared_ptr<Material> mat) {
    return pyramid<Triangle>(base, h, mat);
}