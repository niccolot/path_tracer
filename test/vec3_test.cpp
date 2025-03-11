#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <iostream>

#include "vec3.h"

TEST_CASE("Vec3 basic operations") {
    SECTION("Construction, destruction, overloaded operators") {
        Vec3<float> a;
        Vec3<double> b;
        Vec3f c;
        Vec3Double e;

        c.set_x(1.f);
        e.set_x(1.);
        Vec3f f;
        Vec3Double g;
        f += c;
        g -= e;

        REQUIRE(f.x() == 1.f);
        REQUIRE(g.x() == -1.);
        REQUIRE(typeid(f.x()) == typeid(float));
        REQUIRE(typeid(g.x()) == typeid(double));

        f *= 2;
        REQUIRE(f.x() == 2.f);

        f /= 2;
        REQUIRE(f.x() == 1.f);
    }

    SECTION("Vector functions") {
        Vec3f u(1, 0, 0);
        Vec3f v(0, 1, 0);

        float rel_tol = 0.001f;

        float d = dot(u, v);
        REQUIRE_THAT(d, Catch::Matchers::WithinRel(0.f, rel_tol));

        Vec3f c = cross(u, v);
        REQUIRE(c.length() == c.length_squared());
        REQUIRE_THAT(c.length_squared(), Catch::Matchers::WithinRel(1.f, rel_tol));
        REQUIRE(c.x() == c.y());
        REQUIRE_THAT(c.y(), Catch::Matchers::WithinRel(0.f, rel_tol));
        REQUIRE_THAT(c.z(), Catch::Matchers::WithinRel(1.f, rel_tol));

        Vec3f e(1,2,3);
        auto f = unit_vector(e);
        REQUIRE_THAT(f.length(), Catch::Matchers::WithinRel(1.f, rel_tol));
    }
}