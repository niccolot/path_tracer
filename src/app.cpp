#include "app.h"

SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
    app_context_t context = generate_context();
    *appstate = &context;

    SDL_AppResult success = initialize_app(context);
    if (!success) {
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

app_context_t generate_context() {
    app_context_t context;
    context.img_height = 400;
    context.img_width = 600;

    return context;
}

SDL_AppResult initialize_app(const app_context_t& context)
{
    bool success = SDL_Init(SDL_INIT_VIDEO);
    if (!success) {
        SDL_Log("Init failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    int screen_w, screen_h;
    success = SDL_GetWindowSize(context.window, &screen_w, &screen_h);
    if (!success) {
        SDL_Log("Get Window Size failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    auto window_w = int(screen_w / 3);
    auto window_h = int(screen_h / 2);

    success = SDL_CreateWindowAndRenderer("Path Tracer", window_w, window_h, SDL_WINDOW_RESIZABLE, &context.window, &context.renderer);
    if (!success) {
        SDL_Log("Create Window Failed!: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    success = SDL_SetRenderVSync(context.renderer, 1);
    if (!success) {
        SDL_Log("Failed to set VSync: %s", SDL_GetError());
    }

    context.worker = std::thread{ [&context]() { worker_task(context.queue); } };

    // create a dark surface for background
    context.screen_surface = SDL_CreateSurface(window_w, window_h, SDL_PIXELFORMAT_ARGB32);

    return SDL_APP_CONTINUE;
}