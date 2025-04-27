#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h" 

#include <format>
#include <chrono>

#include "app.h"
#include "mesh.h"
#include "utils.h"

App::App() {
    init_params_t init_pars = init_from_json("init/init_pars.json");
    camera_angles_t angles = angles_from_json("init/camera_angles.json");
    std::vector<geometry_params_t> geometries = geometries_from_json("init/geometry.json");

    _init_pars = init_pars;
    _init_sdl();
    auto outdir = "output/" + Utils::strip_extenstions(_init_pars.outfile_name) + "/";
    Utils::set_directory(outdir);
    _logger = std::make_shared<Logger>(outdir, _init_pars.outfile_name);
    _cam = Camera{ init_pars, angles, geometries, _logger };
    _cam.set_pixel_format(_image_surface->format);
    _cam.set_meshes();
}

void App::_init_sdl() {
    /**
     * @brief: initializes SDL variables,
     * if pixel format is changed also the decoded 
     * components in _save_png() must be changed
     */
    bool success{ SDL_Init( SDL_INIT_VIDEO ) };
    if (!success) {
        throw std::runtime_error{ std::format("SDL failed to initialize: {}\n", SDL_GetError()) };
    }

    success = SDL_CreateWindowAndRenderer("Path Tracer", _init_pars.window_width, _init_pars.window_height, SDL_WINDOW_RESIZABLE, &_window, &_renderer);
    if (!success) {
        throw std::runtime_error{ std::format("SDL failed to create window and renderer: {}\n", SDL_GetError()) };
    }

    success = SDL_SetWindowPosition(_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    if (!success) {
        throw std::runtime_error{ std::format("SDL failed to position the window: {}\n", SDL_GetError()) };
    }

    _image_surface = SDL_CreateSurface(_init_pars.img_width, _init_pars.img_height, SDL_PIXELFORMAT_RGBA8888);
    if (!_image_surface) {
        throw std::runtime_error{ std::format("SDL failed to create image surface: {}\n", SDL_GetError()) };
    }
}

void App::_worker_task() {
    /**
     * @brief: logic for the screen visualization, calls the camera rendering and
     * pushes each row in a buffer. They will be visualized at screen
     * asynchronously
     */
    auto t_start = std::chrono::steady_clock::now();
    uint32_t row_idx = 0;
    while (!_done_rendering) {
        //scanline_t scanline{ row_idx, _cam.render_row(row_idx) };
        _queue.push(scanline_t{ row_idx, _cam.render_row(row_idx) });
        //_queue.push_ref(scanline);
        if (row_idx == _init_pars.img_height - 1) {
            _done_rendering = true;
        }
        row_idx++;
    }

    auto t_end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count();
    _logger->set_rendertime(elapsed / 1000.f);
    _logger->log();
}

void App::_save_png() {
    /**
     * @detail: assuming RGBA8888 big endian pixel format
     */
    std::vector<uint8_t> rgba_pixels(_init_pars.img_width * _init_pars.img_height * 4);
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

    auto img_path = "output/" + Utils::strip_extenstions(_init_pars.outfile_name) + "/" + _init_pars.outfile_name;
    auto ok = stbi_write_png(
        (img_path).c_str(), 
        _init_pars.img_width, 
        _init_pars.img_height, 
        4, 
        rgba_pixels.data(), 
        _init_pars.img_width * 4);

    if (!ok) {
        std::cerr << "\nFailed to save .png file\n"; 
    } else {
        std::cout << std::format("\nRendered image saved as: '{}'\n", img_path);
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
    /**
     * @brief: calls the rendering job in a separate thread and then
     * reconstruct the image on the screen while is being rendered, 
     * a .png file is saved at the end
     */
    _worker = std::thread{ &App::_worker_task, this };
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

        if (_done_rendering && !_img_saved) {
            _save_png();
            _img_saved = true;
        }

        SDL_Event e;
        if (SDL_WaitEventTimeout(&e, _done_rendering ? 100 : 0)) {
            if (e.type == SDL_EVENT_QUIT) {
                _done_rendering = true;
                _quit_app = true;
            }
        }

        SDL_Texture* tex = SDL_CreateTextureFromSurface(_renderer, _image_surface);
        SDL_RenderTexture(_renderer, tex, nullptr, nullptr);
        SDL_DestroyTexture(tex);
        SDL_RenderPresent(_renderer);
    }
}