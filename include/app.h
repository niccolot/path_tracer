#ifndef APP_H
#define APP_H

#include <vector>
#include <atomic>
#include <thread>

#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

#include "multithreading.h"

typedef struct AppContext {
    std::atomic<bool> quit_app = false;
    std::atomic<bool> done_rendering = false;
    uint32_t img_width;
    uint32_t img_height;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::thread worker;
    ThreadSafeQueue<scanline_t> queue;
    SDL_Surface* screen_surface = nullptr;
} app_context_t;

app_context_t generate_context();
SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv);
SDL_AppResult initialize_app(const app_context_t& context);
SDL_AppResult SDL_AppIterate(void* appstate);
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event);
void SDL_AppQuit(void* appstate, SDL_AppResult result);
#endif