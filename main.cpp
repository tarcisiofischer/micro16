#include <array>
#include <micro16.hpp>
#include <sdl_screen.hpp>

int main()
{
    auto code = std::array<Byte, BANK_SIZE>{
        SB_CODE, 0b00000001,
        SETB_CODE, 0b01001111,
        SETB_CODE, 0b01101111,
        SETB_CODE, 0b00001111,
        SETB_CODE, 0b00101111,
        ST_CODE, 0b00000100,
        SETB_CODE, 0b11001010,
        JMP_CODE, 0b00000011
    };

    SDLScreen monitor{};
    Micro16 mcu{code};
    mcu.register_mmio(monitor, Address{0x0000});
    mcu.run();

    return 0;
}
