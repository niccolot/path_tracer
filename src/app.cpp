#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h" 

#include <format>

#include "app.h"
#include "input.h"

App::App(const std::string& file_path) {
    init_params_t init_pars = init_from_json(file_path);

    _img_width = init_pars.img_width;
    _img_height = init_pars.img_height;
    _window_height = init_pars.window_height;
    _window_width = init_pars.window_width;
    _outfile_name = init_pars.outfile_name;

    _init_app();
    _init_cam(init_pars);
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

    _image_surface = SDL_CreateSurface(_img_width, _img_height, SDL_PIXELFORMAT_RGBA8888);
    if (!_image_surface) {
        throw std::runtime_error{ std::format("SDL failed to create image surface: {}\n", SDL_GetError()) };
    }
}

void App::_init_cam(const init_params_t& init_pars) {
    _cam = std::move(Camera{ init_pars });
    _cam.set_pixel_format(_image_surface->format);
}

void App::_worker_task() {
    uint32_t row_idx = 0;
    while (!_done_rendering) {
        // sleep for few millisecond to increase visual smoothness
        // if the rendering is particularly fast
        std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });        
        _queue.push({ row_idx, _cam.render_row(row_idx) });
        if (row_idx == _img_height) {
            _done_rendering = true;
        }
        row_idx++;
    }
}

void App::_save_png() {

    std::vector<uint8_t> rgba_pixels(_img_width * _img_height * 4);
    int pixel_idx = 0;
    for (auto& pair : _pixels_map) {
        auto& row_pixels = pair.second;
        for (auto pixel : row_pixels) {
            uint8_t r = (pixel >> 24) & 0xFF;  
            uint8_t g = (pixel >> 16) & 0xFF;  
            uint8_t b = (pixel >> 8) & 0xFF;   
            uint8_t a = (pixel >> 0) & 0xFF;  
            
            rgba_pixels[pixel_idx++] = r;
            rgba_pixels[pixel_idx++] = g;
            rgba_pixels[pixel_idx++] = b;
            rgba_pixels[pixel_idx++] = a;
        }
    }

    auto ok = stbi_write_png(_outfile_name.c_str(), _img_width, _img_height, 4, rgba_pixels.data(), _img_width * 4);
    if (!ok) {
        std::cerr << "Failed to save .png file\n"; 
    } else {
        std::cout << std::format("Rendered image saved as: '{}'\n", _outfile_name);
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
            _pixels_map[line_val.row] = std::move(line_val.values);
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

    _save_png();
}