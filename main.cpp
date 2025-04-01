#include <iostream>

#include <nlohmann/json.hpp>

#include "app.h"

int main() {
    try {
        App app("init/init_pars.json");
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
    }
}