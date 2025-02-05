#ifndef ONB_H 
#define ONB_H

#include "vec3.h"

class ONB {
    private:
        Vec3 axis[3];

    public:
        ONB() = default;
        ONB(const Vec3& n);

        ONB& operator=(const ONB& base);

        const Vec3& u() const { return axis[0]; }
        const Vec3& v() const { return axis[1]; }
        const Vec3& w() const { return axis[2]; }

        Vec3 transform(const Vec3& v) const {
            /**
             * @brief transform from basis coordinates to local space
             */
            return (v[0]*axis[0]) + (v[1]*axis[1]) + (v[2]*axis[2]);
        }
}; // class ONB
#endif