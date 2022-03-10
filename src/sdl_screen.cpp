#include <sdl_screen.hpp>

namespace {
    struct SDLScopedSurfaceLock {
        SDLScopedSurfaceLock(SDL_Surface* surface)
            : surface(surface)
        {
            SDL_LockSurface(this->surface);
        }

        ~SDLScopedSurfaceLock()
        {
            SDL_UnlockSurface(this->surface);
        }

        SDL_Surface* surface;
    };
}

SDLScreen::SDLScreen()
{
    SDL_Init(SDL_INIT_VIDEO);
    this->window = SDL_CreateWindow(
        "Micro16",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WIDTH,
        HEIGHT,
        0
    );
}

SDLScreen::~SDLScreen()
{
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}

void SDLScreen::update()
{
    if (!this->is_connected()) {
        return;
    }

    SDL_Event event;
    if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
        if (this->on_window_close) {
            this->on_window_close();
        }
        return;
    }

    SDL_Surface* surface = SDL_GetWindowSurface(this->window);
    auto *ptr = (unsigned int *) surface->pixels;
    {
        SDLScopedSurfaceLock _surface_lock{surface};
        std::scoped_lock _{this->video_memory_ptr_mutex};
        auto *video_mem = this->video_memory_ptr;
        if (video_mem == nullptr) {
            return;
        }
        for (int i = 0; i < HEIGHT; ++i) {
            for (int j = 0; j < WIDTH / 2; ++j) {
                auto mem_data = video_mem[WIDTH / 2 * i + j];
                ptr[WIDTH * i + 2 * j + 0] = bits_to_color.at((mem_data & 0x0f) >> 0);
                ptr[WIDTH * i + 2 * j + 1] = bits_to_color.at((mem_data & 0xf0) >> 4);
            }
        }
    }
    SDL_UpdateWindowSurface(this->window);

    // Avoid CPU peak
    SDL_Delay(1);
}

void SDLScreen::connect_to_memory(Byte* memory_start)
{
    std::scoped_lock _{this->video_memory_ptr_mutex};
    this->video_memory_ptr = memory_start;
}

bool SDLScreen::is_connected() const
{
    return this->video_memory_ptr != nullptr;
}

void SDLScreen::disconnect()
{
    std::scoped_lock _{this->video_memory_ptr_mutex};
    this->video_memory_ptr = nullptr;
}

void SDLScreen::register_on_window_close_callback(std::function<void()> const& callback)
{
    this->on_window_close = callback;
}
