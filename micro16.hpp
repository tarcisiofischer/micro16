#ifndef MICRO16_MICRO16_H
#define MICRO16_MICRO16_H

#include <array>
#include <bitset>

using Register = unsigned int;
using Byte = unsigned char;
using Word = unsigned int;
using Address = unsigned int;
using Instruction = unsigned int;

static constexpr auto CODE_BANK = 0;
static constexpr auto MMIO_BANK = 1;
static constexpr auto N_BANKS = 4;
static constexpr auto BANK_SIZE = 64 * 1024;

static constexpr Byte NOP_CODE{0x00};
static constexpr Byte ADD_CODE{0x01};
static constexpr Byte SETB_CODE{0x08};
static constexpr Byte CLR_CODE{0x0A};
static constexpr Byte LD_CODE{0x41};
static constexpr Byte ST_CODE{0x42};
static constexpr Byte JMP_CODE{0x82};
static constexpr Byte SB_CODE{0xB2};
static constexpr Byte HLT_CODE{0xBF};

class Micro16 {
public:
    class Adapter {
    public:
        virtual void connect_to_memory(Byte* memory_start) = 0;
    };
public:
    Micro16(std::array<Byte, BANK_SIZE> const& code);
    void run();
    void register_mmio(Adapter& adapter, Address request_addr);

private:
    Instruction instruction_fetch() const;
    void run_instruction(Instruction const& instruction);

private:
    bool running;

    Register IP;
    Register CR;
    Register SP;
    std::array<Register, 4> W;

    std::array<std::array<Byte, BANK_SIZE>, N_BANKS> memory_banks;
};

#endif //MICRO16_MICRO16_H
