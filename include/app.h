#ifndef APP_H
#define APP_H

#include <vector>
#include <atomic>
#include <thread>
#include <exception>
#include <string>
#include <fstream>
#include <map>

#include "SDL3/SDL.h"

#include "multithreading.h"
#include "camera.h"
#include "vec3.h"
#include "interval.h"

class App {
private:
    std::atomic<bool> _quit_app{ false };
    std::atomic<bool> _done_rendering{ false };
    init_params_t _init_pars;
    SDL_Window* _window{ nullptr };
    SDL_Renderer* _renderer{ nullptr };
    SDL_Surface* _image_surface{ nullptr };
    std::thread _worker;
    std::string _outfile_name;
    std::map<uint32_t, std::vector<uint32_t>> _pixels_map;
    ThreadSafeQueue<scanline_t> _queue;
    Camera _cam;
    void _worker_task();
    void _init_app();
    void _save_png();

public:
    App(const std::string& file_path);
    ~App();

    void run();
}; // class App
#endif