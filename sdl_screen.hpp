#ifndef MICRO16_SDL_SCREEN_HPP
#define MICRO16_SDL_SCREEN_HPP

#include <micro16.hpp>

#include <SDL2/SDL.h>
#include <thread>
#include <array>
#include <unordered_map>

using ColorHex = unsigned int;

static const std::unordered_map<std::bitset<4>, ColorHex> bits_to_color = {
        {0x0, 0x191919}, {0x8, 0xccdb25},
        {0x1, 0xcbcbcb}, {0x9, 0xccdb88},
        {0x2, 0xac3232}, {0xA, 0xd2842a},
        {0x3, 0xac716b}, {0xB, 0xd2ac7a},
        {0x4, 0x4fac43}, {0xC, 0x824aad},
        {0x5, 0x92b687}, {0xD, 0xb795c2},
        {0x6, 0x5b69ac}, {0xE, 0x1f7d6e},
        {0x7, 0xacadc8}, {0xF, 0x87ccc8},
};

class SDLScreen : public Micro16::Adapter {
public:
    static auto constexpr WIDTH = 320;
    static auto constexpr HEIGHT = 200;

    SDLScreen();
    ~SDLScreen();

    static void update_window(SDLScreen* self);
    void connect_to_memory(Byte* memory_start) override;
    void disconnect() override;

private:
    bool connected;
    SDL_Window* window;
    std::thread update_window_thread;
    Byte* video_memory_ptr;
};

#endif //MICRO16_SDL_SCREEN_HPP
