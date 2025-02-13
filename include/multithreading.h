#ifndef MULTITHREADING_H
#define MULTITHREADING_H

#include <vector>

#include "vec3.h"

typedef struct BlockJob {
    int row_start = 0;
    int row_end = 0;
    int row_size;
    std::vector<int> indices;
    std::vector<Vec3> colors;
} block_job_t; // block_job_t 
#endif