#include <iostream>

#include <nlohmann/json.hpp>

#include "app.h"

int main() {
    try {
        //App app(1920, 1080, 960, 540);
        //app.run();
        nlohmann::json jsonData = {{"name", "Alice"}, {"age", 25}, {"city", "Wonderland"}};
        std::cout << jsonData.dump(4) << std::endl;  // Pretty print JSON
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
    }
}