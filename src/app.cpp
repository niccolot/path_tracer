#include <format>

#include "app.h"
#include "input.h"

App::App(uint32_t img_width, uint32_t img_height, uint32_t window_width, uint32_t window_height) {
    _img_width = img_width;
    _img_height = img_height;
    _window_width = window_width;
    _window_height = window_height;

    _init_app();
}

App::App(const std::string& file_path) {
    init_params_t init_pars = init_from_json(file_path);

    _img_width = init_pars.img_width;
    _img_height = init_pars.img_height;
    _window_height = init_pars.window_height;
    _window_width = init_pars.window_width;
    _lookfrom = init_pars.lookfrom;
    _lookat = init_pars.lookat;
    _background = init_pars.background;
    _vfov = init_pars.vfov;
    _focus_dist = init_pars.focus_dist;

    _init_app();
}

void App::_init_app() {
    bool success{ SDL_Init( SDL_INIT_VIDEO ) };
    if (!success) {
        throw std::runtime_error{ std::format("SDL failed to initialize: {}\n", SDL_GetError()) };
    }

    success = SDL_CreateWindowAndRenderer("Path Tracer", _window_width, _window_height, SDL_WINDOW_RESIZABLE, &_window, &_renderer);
    if (!success) {
        throw std::runtime_error{ std::format("SDL failed to create window and renderer: {}\n", SDL_GetError()) };
    }

    success = SDL_SetWindowPosition(_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    if (!success) {
        throw std::runtime_error{ std::format("SDL failed to position the window: {}\n", SDL_GetError()) };
    }

    _image_surface = SDL_CreateSurface(_img_width, _img_height, SDL_PIXELFORMAT_ARGB32);
    if (!_image_surface) {
        throw std::runtime_error{ std::format("SDL failed to create image surface: {}\n", SDL_GetError()) };
    }

    _cam = std::move(Camera{_img_width, _img_height});
}

void App::_worker_task() {
    uint32_t row = 0;
    while (!_done_rendering) {
        // sleep for few millisecond to increase visual smoothness
        // if the rendering is particularly fast
        std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });        
        _queue.push({ row, std::move(_cam.render_row(row)) });
        
        if (row == _img_height) {
            _done_rendering = true;
        }
        row++;
    }
}

App::~App() {
    if (_renderer) {
        SDL_DestroyRenderer(_renderer);
    }
    SDL_DestroySurface(_image_surface);
    if (_window) {
        SDL_DestroyWindow(_window);
    }
    if (_worker.joinable()) {
        _worker.join();
    }
}

void App::run() {
    _worker = std::thread{ &App::_worker_task, this};
    while(!_quit_app) {
        std::optional<scanline_t> line = _queue.try_pop();
        while (line && !_done_rendering) {
            scanline_t line_val = line.value();
            uint32_t* pixels = static_cast<uint32_t*>(_image_surface->pixels);
            size_t begin_pixel = _image_surface->pitch / sizeof(uint32_t) * line_val.row;
            for (size_t i = 0; i < line_val.values.size(); i++) {
                pixels[begin_pixel + i] = line_val.values[i];
            }
            line = _queue.try_pop();
        }

        SDL_Event e;
        while (SDL_PollEvent(&e)) {
                if (e.type == SDL_EVENT_QUIT) {
                    _quit_app = true;
                }
        }

        SDL_Texture* tex = SDL_CreateTextureFromSurface(_renderer, _image_surface);
        SDL_RenderTexture(_renderer, tex, nullptr, nullptr);
        SDL_DestroyTexture(tex);
        SDL_RenderPresent(_renderer);
    }
}