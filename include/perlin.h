#ifndef PERLIN_H
#define PERLIN_H

#include "vec3.h"

class Perlin {
    private:
        static const int point_count = 256;
        double randfloat[point_count];
        int perm_x[point_count];
        int perm_y[point_count];
        int perm_z[point_count];

        static void perlin_generate_perm(int* p);
        static void permute(int* p, int n);
        static double trilinear_interp(double c[2][2][2], double u, double v, double w);
    
    public:
        Perlin();

        double noise(const Vec3& p) const;
}; // class Perlin
#endif