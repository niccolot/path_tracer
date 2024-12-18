#include "pdf.h"

double MixturePDF::value(const Vec3& direction) const {
    double v0{};
    double v1{};
    
    if (p[0] != nullptr) {
        v0 = p[0]->value(direction);
    }

    if (p[1] != nullptr) {
        v1 = p[1]->value(direction);
    } 

    return 0.5 * v0 + 0.5 * v1;
}

Vec3 MixturePDF::generate() const {
    bool p0_null = p[0] == nullptr;
    bool p1_null = p[1] == nullptr;

    int state = (p0_null ? 1 : 0) | (p1_null ? 2 : 0);

    switch (state) {
        case (3): // (nullptr, nullptr)
            return random_unit_vector();
        
        case (2): // (ok, nullptr)
            return p[0]->generate();

        case (1): // (nullptr, ok)
            return p[1]->generate();

        default: // (ok, ok)
            return random_double() < 0.5 ? p[0]->generate() : p[1]->generate();
        } 
}