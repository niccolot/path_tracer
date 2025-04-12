#include <iostream>

#include "app.h"

int main() {
    try {
        App app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
    }
}
