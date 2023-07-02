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
        SCALE * WIDTH,
        SCALE * HEIGHT,
        0
    );
}

SDLScreen::~SDLScreen()
{
    SDL_DestroyWindow(this->window);
    SDL_Quit();
}

inline void SDLScreen::paint_pixel(unsigned int* ptr, int i, int j, Nibble const& pixel_nibble)
{
    for (int k = 0; k < SCALE; ++k) {
        for (int l = 0; l < SCALE; ++l) {
            ptr[(SCALE * SCALE * WIDTH * i + SCALE * j) + (SCALE * WIDTH * l + k)] = bits_to_color.at(pixel_nibble);
        }
    }
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
        auto constexpr N_BYTES_Y = HEIGHT;
        auto constexpr N_BYTES_X = WIDTH / 2;
        for (int i = 0; i < N_BYTES_Y; ++i) {
            for (int j = 0; j < N_BYTES_X; ++j) {
                auto mem_data = video_mem[N_BYTES_X * i + j];
                auto left_nibble = (mem_data & 0xf0) >> 4;
                auto right_nibble = (mem_data & 0x0f) >> 0;
                paint_pixel(ptr, i, 2 * j + 0, left_nibble);
                paint_pixel(ptr, i, 2 * j + 1, right_nibble);
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
