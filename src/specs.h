#ifndef MICRO16_SPECS_H
#define MICRO16_SPECS_H

// TODO: Add more suitable data structures for those. I tried std::bitset, but it uses a `long int` for storage, so
//       for small bitsets it is not "memory friendly" (Although it probably make sense for memory alignment?).
//       But would be better to use it from the typing perspective (Today, a Byte and a Nibble are "the same thing").
using Register = unsigned int;
using Byte = unsigned char;
using Nibble = unsigned char; // Only 4bits used
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
