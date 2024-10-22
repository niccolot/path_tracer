#include "perlin.h"
#include "utils.h"

Perlin::Perlin() {
    for (int i=0; i<point_count; ++i) {
        randfloat[i] = random_double();
    }

    perlin_generate_perm(perm_x);
    perlin_generate_perm(perm_y);
    perlin_generate_perm(perm_z);
}

double Perlin::noise(const Vec3& p) const {
    /**
     * @brief perlin noise with trilinear interpolation
     * and hermitian smoothing
     */
    auto u = p.x() - std::floor(p.x());
    auto v = p.y() - std::floor(p.y());
    auto w = p.z() - std::floor(p.z());
    u = u*u*(3-2*u);
    v = v*v*(3-2*v);
    w = w*w*(3-2*w);

    auto i = int(std::floor(p.x()));
    auto j = int(std::floor(p.y()));
    auto k = int(std::floor(p.z()));
    double c[2][2][2];

    for (int di=0; di<2; ++di) {
        for (int dj=0; dj<2; ++dj) {
            for (int dk=0; dk<2; ++dk) {
                c[di][dk][dj] = randfloat[
                    perm_x[(i+di) & 255] ^
                    perm_y[(j+dj) & 255] ^
                    perm_z[(k+dk) & 255]
                ];
            }
        }
    }

    return trilinear_interp(c,u,v,w);
}

void Perlin::perlin_generate_perm(int* p) {
    for (int i=0; i<point_count; ++i) {
        p[i] = i;

        permute(p, point_count);
    }
}

void Perlin::permute(int* p, int n) {
    for (int i=n-1; i>0; --i) {
        int target = random_int(0, i);
        int tmp = p[i];
        p[i] = p[target];
        p[target] = tmp;
    }
}

double Perlin::trilinear_interp(double c[2][2][2], double u, double v, double w) {
    auto accum = 0.;
    for (int i=0; i<2; ++i) {
        for (int j=0; j<2; ++j) {
            for (int k=0; k<2; ++k) {
                accum += (i*u + (1-i)*(1-u))
                       * (j*v + (1-j)*(1-v))
                       * (k*w + (1-k)*(1-w))
                       * c[i][j][k]; 
            }
        }
    }

    return accum;
}