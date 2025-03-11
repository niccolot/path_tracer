#include<iostream>

#include "camera.h"

int main() {
    Camera cam(480, 16.f / 9.f);
    cam.set_background(Color(0.5f, 0.7f, 1.f));
    cam.render();
}