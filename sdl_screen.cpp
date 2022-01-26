#include <sdl_screen.hpp>

SDLScreen::SDLScreen()
    : connected(false)
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

void SDLScreen::update_window(SDLScreen* self)
{
    SDL_Event event;
    SDL_Surface* surface = SDL_GetWindowSurface(self->window);
    while (1) {
        SDL_LockSurface(surface);
        auto *ptr = (unsigned int *) surface->pixels;
        for (int i = 0; i < HEIGHT; ++i) {
            for (int j = 0; j < WIDTH / 2; ++j) {
                auto mem_data = self->video_memory_ptr[WIDTH / 2 * i + j];
                ptr[WIDTH * i + 2 * j + 0] = bits_to_color.at((mem_data & 0x0f) >> 0);
                ptr[WIDTH * i + 2 * j + 1] = bits_to_color.at((mem_data & 0xf0) >> 4);
            }
        }
        SDL_UnlockSurface(surface);
        SDL_UpdateWindowSurface(self->window);

        if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
            return;
        }

        if (!self->connected) {
            return;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void SDLScreen::connect_to_memory(Byte* memory_start)
{
    this->video_memory_ptr = memory_start;
    this->connected = true;
    this->update_window_thread = std::thread{SDLScreen::update_window, this};
}

void SDLScreen::disconnect()
{
    this->connected = false;
    this->update_window_thread.join();
    this->video_memory_ptr = nullptr;
}
