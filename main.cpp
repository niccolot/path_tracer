#include <iostream>

#include "app.h"

int main() {
    try {
        App app(1920, 1080);
        app.run();
    } catch (const SDL_Exception& e) {
        std::cerr << e.what() << "\n";
    }
}