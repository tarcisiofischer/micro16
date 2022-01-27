#include <array>
#include <micro16.hpp>
#include <sdl_screen.hpp>

std::array<Byte, BANK_SIZE> led_blink_code()
{
    auto led_blink_code = std::array<Byte, BANK_SIZE>{
            // BEGIN Setup IT
            // Select Bank 1 (where IT is)
/*0x00*/    SELB_CODE, 0b00000001,
            // W[0] = 0x1000
/*0x02*/    SET_CODE, 0b00110001,
            // W[1] = 0x7d04 (Timer 1 IT addr)
/*0x04*/    SET_CODE, 0b01110111,
/*0x06*/    SET_CODE, 0b01101101,
/*0x08*/    SET_CODE, 0b01010000,
/*0x0A*/    SET_CODE, 0b01000100,
            // Set Timer 1 JMP = 0x1000
/*0x0C*/    ST_CODE, 0b00000100,
            // END Setup IT

            // Enable Timer 1 interrupt (500ms timer)
/*0x0E*/    ETI_CODE, 0b00000001,
/*0x10*/    EAI_CODE, 0b00000000,

            // Busy wait
/*0x12*/    SET_CODE, 0b11110000, // 0
/*0x14*/    SET_CODE, 0b11100000, // 0
/*0x16*/    SET_CODE, 0b11010001, // 1
/*0x18*/    SET_CODE, 0b11001010, // A
/*0x1A*/    JMP_CODE, 0b00000011,
    };

    auto addr = 0x1000;
                // W[1] = 0x1010
/* 0x1000 */    led_blink_code[addr++] = SET_CODE;   led_blink_code[addr++] = 0b01110001;
/* 0x1002 */    led_blink_code[addr++] = SET_CODE;   led_blink_code[addr++] = 0b01100000;
/* 0x1004 */    led_blink_code[addr++] = SET_CODE;   led_blink_code[addr++] = 0b01010001;
/* 0x1006 */    led_blink_code[addr++] = SET_CODE;   led_blink_code[addr++] = 0b01000000;
                // W[0] = led_ptr
/* 0x1008 */    led_blink_code[addr++] = SET_CODE;   led_blink_code[addr++] = 0b00110111;
/* 0x100A */    led_blink_code[addr++] = SET_CODE;   led_blink_code[addr++] = 0b00101100;
/* 0x100C */    led_blink_code[addr++] = SET_CODE;   led_blink_code[addr++] = 0b00011111;
/* 0x100E */    led_blink_code[addr++] = SET_CODE;   led_blink_code[addr++] = 0b00001110;
                // W[2] = *led_ptr
/* 0x1010 */    led_blink_code[addr++] = LD_CODE;    led_blink_code[addr++] = 0b00000010;
                // W[2] = ~W[2]
/* 0x1012 */    led_blink_code[addr++] = NOT_CODE;   led_blink_code[addr++] = 0b00000010;
                // *led_ptr = W[0]
/* 0x1014 */    led_blink_code[addr++] = ST_CODE;    led_blink_code[addr++] = 0b00000010;
/* 0x1016 */    led_blink_code[addr++] = DEC_CODE;   led_blink_code[addr++] = 0b00000000;
/* 0x1016 */    led_blink_code[addr++] = DEC_CODE;   led_blink_code[addr++] = 0b00000000;
/* 0x1018 */    led_blink_code[addr++] = BRNZ_CODE;  led_blink_code[addr++] = 0b00000100;
/* 0x101A */    led_blink_code[addr++] = RETI_CODE;  led_blink_code[addr++] = 0b00000000;

    return led_blink_code;
}

std::array<Byte, BANK_SIZE> draw_line_code()
{
    auto draw_line_code = std::array<Byte, BANK_SIZE>{
            // Select video memory bank
/*0x00*/    SELB_CODE, 0b00000001,
            // W[1] = 0xFFFF
/*0x02*/    SET_CODE, 0b01111111,
/*0x04*/    SET_CODE, 0b01101111,
/*0x06*/    SET_CODE, 0b01011111,
/*0x08*/    SET_CODE, 0b01001111,
            // W[0] = 0x0F0F
/*0x0A*/    SET_CODE, 0b00110000,
/*0x0C*/    SET_CODE, 0b00100000,
/*0x0E*/    SET_CODE, 0b00010000,
/*0x10*/    SET_CODE, 0b00000000,
            // Draw line on screen
/*0x12*/    ST_CODE, 0b00000001,
            // Busy loop (Do nothing)
/*0x14*/    SET_CODE, 0b11110000,
/*0x16*/    SET_CODE, 0b11100000,
/*0x18*/    SET_CODE, 0b11010001,
/*0x1A*/    SET_CODE, 0b11001100,
/*0x1C*/    JMP_CODE, 0b00000011
    };

    return draw_line_code;
}

int main()
{
    auto code = led_blink_code();

    Micro16 mcu{code};
    SDLScreen monitor{};
    mcu.register_mmio(monitor, Address{0x0000});
    mcu.run();

    return 0;
}
