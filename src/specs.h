#ifndef MICRO16_SPECS_H
#define MICRO16_SPECS_H

using Register = unsigned int;
using Byte = unsigned char;
using Word = unsigned int;
using Address = unsigned int;
using Instruction = unsigned int;

static constexpr auto CODE_BANK = 0;
static constexpr auto MMIO_BANK = 1;
static constexpr auto IT_BANK = 1;
static constexpr auto N_BANKS = 4;
static constexpr auto BANK_SIZE = 64 * 1024;

static constexpr auto IT_ADDR = 0x7d00;

#endif //MICRO16_SPECS_H
