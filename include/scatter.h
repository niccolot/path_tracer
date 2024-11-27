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
    Color attenuation;
} scattered_rays_t; // scattered_rays_t

typedef struct ScatterRecord {
    std::vector<scattered_rays_t> scattered_rays;
} scatter_record_t; // scatter_record_t
#endif