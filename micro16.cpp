#include <micro16.hpp>

Micro16::Micro16(std::array<Byte, BANK_SIZE> const& code)
    : running(true)
    , IP(0x0000)
    , CR(0x9000)
    , SP(0x8000)
    , W({0x0000, 0x0000, 0x0000, 0x0000})
{
    memory_banks[0] = code;
}

void Micro16::run()
{
    while (1) {
        auto instruction = this->instruction_fetch();
        this->run_instruction(instruction);
        if (!this->running) {
            break;
        }
    }

    for (auto adapter : this->adapters) {
        adapter->disconnect();
    }
}

Instruction Micro16::instruction_fetch() const
{
    auto left = this->memory_banks[CODE_BANK][this->IP];
    auto right = this->memory_banks[CODE_BANK][this->IP + 1];

    return (left << 8) + right;
}

void Micro16::register_mmio(Adapter& adapter, Address request_addr)
{
    auto* mem_addr = &this->memory_banks[MMIO_BANK][request_addr];
    adapter.connect_to_memory(mem_addr);
    this->adapters.push_back(&adapter);
}

void Micro16::run_instruction(Instruction const& instruction)
{
    auto instruction_code = static_cast<Byte>((instruction & 0xff00) >> 8);
    auto instruction_data = static_cast<Byte>(instruction & 0x00ff);

    bool IP_changed = false;

    if (instruction_code == NOP_CODE) {
    } else if (instruction_code == ADD_CODE) {
        auto aa = (instruction_data & 0b00001100) >> 2;
        auto bb = (instruction_data & 0b00000011) >> 0;
        auto cc = (instruction_data & 0b00110000) >> 4;

        this->W[cc] = this->W[aa] + this->W[bb];
    } else if (instruction_code == SETB_CODE) {
        auto aa = (instruction_data & 0b11000000) >> 6;
        auto yy = (instruction_data & 0b00110000) >> 4;
        auto xxxx = (instruction_data & 0b00001111) >> 0;

        W[aa] &= ~(0x000F << (4 * yy));
        W[aa] |= xxxx << (4 * yy);
    } else if (instruction_code == CLR_CODE) {
        auto aa = (instruction_data & 0b00000011) >> 0;

        W[aa] = 0;
    } else if (instruction_code == JMP_CODE) {
        auto aa = (instruction_data & 0b00000011) >> 0;

        this->IP = W[aa];
        IP_changed = true;
    } else if (instruction_code == LD_CODE) {
        // TODO
    } else if (instruction_code == ST_CODE) {
        auto selected_bank = (this->CR & 0x3000) >> 12;
        auto aa = (instruction_data & 0b00001100) >> 2;
        auto bb = (instruction_data & 0b00000011) >> 0;

        this->memory_banks[selected_bank][this->W[aa]] = (this->W[bb] & 0xff00) >> 8;
        this->memory_banks[selected_bank][this->W[aa] + 1] = (this->W[bb] & 0x00ff) >> 0;
    } else if (instruction_code == HLT_CODE) {
        this->running = false;
    }

    if (!IP_changed) {
        this->IP += 2;
    }
    if (this->IP >= BANK_SIZE) {
        this->IP = 0;
    }
}