#include <iostream>

#include "image.h"

int main() {

    int img_width = 400;
    double aspect_ratio = 16.0 / 9.0;

    render_image(std::cout, img_width, aspect_ratio);
}