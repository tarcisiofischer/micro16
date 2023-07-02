#ifndef MICRO16_SPECS_H
#define MICRO16_SPECS_H

#include <cstdint>

using Register = uint16_t;
using Byte = uint8_t;
using Nibble = uint8_t; // Only 4bits used
using Address = uint16_t;
using Instruction = uint16_t;

static constexpr auto CODE_BANK = 0;
static constexpr auto MMIO_BANK = 1;
static constexpr auto IT_BANK = 1;
static constexpr auto N_BANKS = 4;
static constexpr auto BANK_SIZE = 64 * 1024;

static constexpr auto IT_ADDR = 0x7d00;

#endif //MICRO16_SPECS_H
