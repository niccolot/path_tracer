#ifndef SCATTER_H
#define SCATTER_H

#include <memory>
#include <vector>

#include "color.h"
#include "pdf.h"

typedef struct ScatteredRays {
    std::shared_ptr<PDF> pdf;
    bool skip_pdf;
    Ray skip_pdf_ray;
} scattered_rays_t; // scattered_rays_t

class ScatterRecord {
    public:
        Color attenuation;
        std::vector<scattered_rays_t> scattered_rays;
}; // class ScatterRecord
#endif