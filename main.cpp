#include "SDL3/SDL.h"
#define SDL_MAIN_USE_CALLBACKS
#include "SDL3/SDL_main.h"

#include <queue>
#include <mutex>
#include <optional>
#include <random>
#include <thread>

#include "camera.h"
#include "multithreading.h"

std::atomic<bool> quit_app = false;
std::atomic<bool> done_rendering = false;
constexpr uint32_t width = 1920;
constexpr uint32_t height = 1080;

struct ScanLine
{
    uint32_t row;
    std::vector<uint32_t> values;
};

// compute scanlines and push them on the queue
void worker_task(ThreadSafeQueue<ScanLine>& queue)
{
    uint32_t row = 0;
    std::mt19937 engine;
    std::uniform_int_distribution dist{};
    Camera cam(width, height);
    while (!quit_app)
    {
        // sleep for few millisecond
        std::this_thread::sleep_for(std::chrono::milliseconds{ 1 });        
        // push result to queue
        queue.push({ row, std::move(cam.render_row(row)) });
        
        if (row == height) {
            done_rendering = true;
        }
        row++;
    }
}

struct AppContext
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    std::thread worker;
    ThreadSafeQueue<ScanLine> queue;
    SDL_Surface* screen_surface = nullptr;
};

// replaces main, called on startup
SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
{
    AppContext* context = new AppContext{};
    *appstate = context;

    bool success = SDL_Init(SDL_INIT_VIDEO);
    if (!success)
    {
        SDL_Log("Init failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    success = SDL_CreateWindowAndRenderer("My Window!", width, height, SDL_WINDOW_RESIZABLE, &context->window, &context->renderer);
    if (!success)
    {
        SDL_Log("Create Window Failed!: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    // set VSYNC
    success = SDL_SetRenderVSync(context->renderer, 1);
    if (!success)
    {
        SDL_Log("Failed to set VSync: %s", SDL_GetError());
    }

    // create worker
    context->worker = std::thread{ [&context]() { worker_task(context->queue); } };

    // create a dark surface
    context->screen_surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_ARGB32);

    return SDL_APP_CONTINUE;
}

// called every frame
SDL_AppResult SDL_AppIterate(void* appstate)
{
    AppContext* context = (AppContext*)appstate;

    SDL_RenderClear(context->renderer);

    // update surface from other thread data
    auto line = context->queue.try_pop();
    while (line && !done_rendering)
    {
        uint32_t* pixels = static_cast<uint32_t*>(context->screen_surface->pixels);
        size_t begin_pixel = context->screen_surface->pitch / sizeof(uint32_t) * line->row;
        for (size_t i = 0; i < line->values.size(); i++)
        {
            pixels[begin_pixel + i] = line->values[i];
        }
        line = context->queue.try_pop();
    }

    // render surface to screen
    SDL_Texture* tex = SDL_CreateTextureFromSurface(context->renderer, context->screen_surface);
    SDL_RenderTexture(context->renderer, tex, nullptr, nullptr);
    SDL_DestroyTexture(tex);

    SDL_RenderPresent(context->renderer);

    return SDL_APP_CONTINUE;
}

// event handling here
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
    if (event->type == SDL_EVENT_QUIT)
    {
        quit_app = true;
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

// called when app ends, cleanup
void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
    AppContext* context = (AppContext*)appstate;
    // clean-up
    if (context->worker.joinable())
    {
        // this should be join not detach ... but it is up to you
        context->worker.detach();
    }
    if (context->screen_surface)
    {
        SDL_DestroySurface(context->screen_surface);
    }
    if (context->renderer)
    {
        SDL_DestroyRenderer(context->renderer);
    }
    if (context->window)
    {
        SDL_DestroyWindow(context->window);
    }
    delete context;
}