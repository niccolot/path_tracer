#include <iostream>

int main() {

    int img_width = 256;
    int img_height = 256;

    std::cout << "P3\n" << img_width << ' ' << img_height << "\n255\n";

    for (int j=0; j<img_height; ++j) {
        std::clog << "\rScanlines remaining: " << (img_height - j) << ' ' << std::flush;
        for (int i=0; i<img_width; ++i) {
            auto r = double(i) / (img_width-1);
            auto g = double(j) / (img_height-1);
            auto b = 0.0;

            int ir = int(255.999 * r);
            int ig = int(255.999 * g);
            int ib = int(255.999 * b);

            std::cout << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }

    std::clog << "\rDone.       \n";
}