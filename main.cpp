#include <iostream>

#include <nlohmann/json.hpp>

#include "app.h"

int main() {
    //try {
    //    App app("init");
    //    app.run();
    //} catch (const std::exception& e) {
    //    std::cerr << e.what() << "\n";
    //}
    objl::Loader loader;
    bool ok = loader.LoadFile("init/box_stack.obj");
    if (!ok) {
        std::cerr << "failed to load obj file\n"; 
    } else {
        Mesh mesh(loader.LoadedMeshes[0]);
    }
}
