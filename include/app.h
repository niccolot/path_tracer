#ifndef APP_H
#define APP_H

#include <vector>
#include <atomic>
#include <thread>
#include <exception>
#include <string>
#include <string_view>

#include "SDL3/SDL.h"

#include "multithreading.h"
#include "camera.h"
#include "vec3.h"

typedef struct InitParams {
    uint32_t img_width;
    uint32_t img_height;
    uint32_t window_width;
    uint32_t window_height;
    Vec3f lookfrom;
    Vec3f lookat;
    Color background;
    float vfov;
    float focus_dist;
} init_params_t;

class App {
private:
    std::atomic<bool> _quit_app{ false };
    std::atomic<bool> _done_rendering{ false };
    uint32_t _img_width;
    uint32_t _img_height;
    uint32_t _window_width;
    uint32_t _window_height;
    SDL_Window* _window{ nullptr };
    SDL_Renderer* _renderer{ nullptr };
    SDL_Surface* _image_surface{ nullptr };
    std::thread _worker;
    ThreadSafeQueue<scanline_t> _queue;
    Camera _cam;
    void _worker_task();
    void _init_app();

public:
    App(uint32_t img_width, uint32_t img_height, uint32_t window_width, uint32_t window_height);
    App(const std::string& file_path);
    ~App();
    void run();
}; // class App
#endif