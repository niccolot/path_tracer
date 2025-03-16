#include "app.h"

App::App(uint32_t w, uint32_t h) {
    _img_width = w;
    _img_height = h;
    _window_width = w;
    _window_height = h;

    bool success{ SDL_Init( SDL_INIT_VIDEO ) };
    if (!success) {
        SDL_Log("SDL failed to initialize: %s\n", SDL_GetError());
        throw SDL_Exception("SDL failed to initialize");
    }

    success = SDL_CreateWindowAndRenderer("Path Tracer", _window_width, _window_height, SDL_WINDOW_RESIZABLE, &_window, &_renderer);
    if (!success) {
        SDL_Log("SDL failed to create window and renderer: %s", SDL_GetError());
        throw SDL_Exception("SDL failed to create window and renderer");
    }

    _screen_surface = SDL_CreateSurface(_window_width, _window_height, SDL_PIXELFORMAT_ARGB32);
    if (!_screen_surface) {
        SDL_Log("SDL failed to create image surface: %s", SDL_GetError());
    }

    _cam = std::move(Camera{_img_width, _img_height});
    _worker = std::thread{ &App::_worker_task, this};
}

void App::_worker_task() {
    uint32_t row = 0;
    while (!_quit_app) {
        // sleep for few millisecond to increase smoothness
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
    SDL_DestroySurface(_screen_surface);
    if (_window) {
        SDL_DestroyWindow(_window);
    }
    if (_worker.joinable()) {
        _worker.join();
    }
}

void App::run() {
    while(!_quit_app) {
        std::optional<scanline_t> line = _queue.try_pop();
        while (line && !_done_rendering) {
            scanline_t line_val = line.value();
            uint32_t* pixels = static_cast<uint32_t*>(_screen_surface->pixels);
            size_t begin_pixel = _screen_surface->pitch / sizeof(uint32_t) * line_val.row;
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
        
        SDL_Texture* tex = SDL_CreateTextureFromSurface(_renderer, _screen_surface);
        SDL_RenderTexture(_renderer, tex, nullptr, nullptr);
        SDL_DestroyTexture(tex);
        SDL_RenderPresent(_renderer);
    }
}