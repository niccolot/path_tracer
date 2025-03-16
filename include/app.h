#ifndef APP_H
#define APP_H

#include <vector>
#include <atomic>
#include <thread>
#include <exception>
#include <string>
#include <string_view>

#include "SDL3/SDL.h"
//#include "SDL3/SDL_main.h"

#include "multithreading.h"
#include "camera.h"

class SDL_Exception : public std::exception {
private:
    std::string _error_msg{};

public:
    SDL_Exception(std::string_view msg) : _error_msg(msg) {}
    const char* what() const noexcept override { return _error_msg.c_str(); }
}; // class SDL_Exception

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
    SDL_Surface* _screen_surface{ nullptr };
    std::thread _worker;
    ThreadSafeQueue<scanline_t> _queue;
    Camera _cam;
    void _worker_task();

public:
    App(uint32_t w, uint32_t h);
    ~App();
    void run();
}; // class App
#endif