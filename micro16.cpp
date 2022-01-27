#include <micro16.hpp>

Micro16::Micro16(std::array<Byte, BANK_SIZE> const& code)
    : running(true)
    , IP(0x0000)
    , CR(0x9000)
    , SP(0x8000)
    , W({0x0000, 0x0000, 0x0000, 0x0000})
    , timer_triggered{false}
    , triggered_timer_id{-1}
    , timer0{*this, 0}
    , timer1{*this, 1}
{
    memory_banks[0] = code;
}

void Micro16::run()
{
    while (true) {
        this->check_interrupts();
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

void Micro16::check_interrupts()
{
    std::scoped_lock _{this->timer_mutex};
    if (this->timer_triggered) {

        // Disable global interrupts
        this->CR &= ~(0x0008);

        // Save current IP on the stack
        auto stack_bank = (this->CR & 0x3000) >> 12;
        this->SP = this->SP + 2;
        this->memory_banks[stack_bank][this->SP] = (this->IP & 0xff00) >> 8;
        this->memory_banks[stack_bank][this->SP + 1] = (this->IP & 0x00ff) >> 0;

        // Jump to the address given in the IT
        auto timer_id = this->triggered_timer_id;
        auto jmp_addr_raw_ptr = &(this->memory_banks[IT_BANK][IT_ADDR + 4 * timer_id]);
        auto jmp_addr = (*(jmp_addr_raw_ptr + 0) << 8) + (*(jmp_addr_raw_ptr + 1) << 0);
        this->IP = jmp_addr;

        this->timer_triggered = false;
        this->triggered_timer_id = -1;
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
    } else if (instruction_code == SUB_CODE) {
        // TODO
    } else if (instruction_code == AND_CODE) {
        // TODO
    } else if (instruction_code == OR_CODE) {
        // TODO
    } else if (instruction_code == XOR_CODE) {
        // TODO
    } else if (instruction_code == INC_CODE) {
        auto aa = (instruction_data & 0b00000011) >> 0;

        W[aa] += 1;
    } else if (instruction_code == DEC_CODE) {
        auto aa = (instruction_data & 0b00000011) >> 0;

        W[aa] -= 1;
    } else if (instruction_code == SET_CODE) {
        auto aa = (instruction_data & 0b11000000) >> 6;
        auto yy = (instruction_data & 0b00110000) >> 4;
        auto xxxx = (instruction_data & 0b00001111) >> 0;

        W[aa] &= ~(0x000F << (4 * yy));
        W[aa] |= xxxx << (4 * yy);
    } else if (instruction_code == CLR_CODE) {
        auto aa = (instruction_data & 0b00000011) >> 0;

        W[aa] = 0;
    } else if (instruction_code == NOT_CODE) {
        auto aa = (instruction_data & 0b00000011) >> 0;

        W[aa] = ~W[aa];
    } else if (instruction_code == JMP_CODE) {
        auto aa = (instruction_data & 0b00000011) >> 0;

        this->IP = W[aa];
        IP_changed = true;
    } else if (instruction_code == BRE_CODE) {
        // TODO
    } else if (instruction_code == BRNE_CODE) {
        // TODO
    } else if (instruction_code == BRL_CODE) {
        // TODO
    } else if (instruction_code == BRH_CODE) {
        // TODO
    } else if (instruction_code == CALL_CODE) {
        // TODO
    } else if (instruction_code == BRNZ_CODE) {
        auto aa = (instruction_data & 0b00000011) >> 0;
        auto cc = (instruction_data & 0b00001100) >> 2;

        if (W[aa] != 0) {
            this->IP = W[cc];
            IP_changed = true;
        }
    } else if (instruction_code == RET_CODE) {
        auto stack_bank = (this->CR & 0x3000) >> 12;
        auto raw_data_ptr = &(this->memory_banks[stack_bank][this->SP]);

        this->IP = (*(raw_data_ptr + 0) << 8) + (*(raw_data_ptr + 1) << 0);
        this->SP = this->SP - 2;

        IP_changed = true;
    } else if (instruction_code == RETI_CODE) {
        auto stack_bank = (this->CR & 0x3000) >> 12;
        auto raw_data_ptr = &(this->memory_banks[stack_bank][this->SP]);

        this->IP = (*(raw_data_ptr + 0) << 8) + (*(raw_data_ptr + 1) << 0);
        this->SP = this->SP - 2;
        // Reenable interrupts
        this->CR |= 0x0008;

        IP_changed = true;
    } else if (instruction_code == LD_CODE) {
        auto selected_bank = (this->CR & 0x3000) >> 12;
        auto aa = (instruction_data & 0b00001100) >> 2;
        auto bb = (instruction_data & 0b00000011) >> 0;

        auto value_ptr = &(this->memory_banks[selected_bank][this->W[aa]]);
        this->W[bb] = (*(value_ptr + 0) << 8) + (*(value_ptr + 1) << 0);
    } else if (instruction_code == ST_CODE) {
        auto selected_bank = (this->CR & 0x3000) >> 12;
        auto aa = (instruction_data & 0b00001100) >> 2;
        auto bb = (instruction_data & 0b00000011) >> 0;

        this->memory_banks[selected_bank][this->W[aa]] = (this->W[bb] & 0xff00) >> 8;
        this->memory_banks[selected_bank][this->W[aa] + 1] = (this->W[bb] & 0x00ff) >> 0;
    } else if (instruction_code == MV_CODE) {
        // TODO
    } else if (instruction_code == PUSH_CODE) {
        // TODO
    } else if (instruction_code == POP_CODE) {
        // TODO
    } else if (instruction_code == DAI_CODE) {
        this->CR &= ~(0x0008);
    } else if (instruction_code == EAI_CODE) {
        this->CR |= 0x0008;
    } else if (instruction_code == DTI_CODE) {
        auto a = (instruction_data & 0b00000001) >> 0;

        this->CR &= ~(0x0100 << a);
    } else if (instruction_code == ETI_CODE) {
        auto a = (instruction_data & 0b00000001) >> 0;

        this->CR |= (0x0100 << a);
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

Micro16::TimerInterruptHandler::TimerInterruptHandler(Micro16& mcu, int timer_id)
    : mcu{mcu}
    , timer_id{timer_id}
    , thread{runner, this}
{
}

Micro16::TimerInterruptHandler::~TimerInterruptHandler()
{
    if (!this->mcu.running) {
        this->thread.join();
    }
}

void Micro16::TimerInterruptHandler::runner(Micro16::TimerInterruptHandler* self)
{
    while (true) {
        auto& mcu = self->mcu;
        auto& timer_id = self->timer_id;

        {
            std::scoped_lock _{mcu.timer_mutex};

            if (
                !mcu.timer_triggered &&
                mcu.CR & 0x0008 &&
                mcu.CR & (1 << (8 + timer_id))
            ) {
                mcu.timer_triggered = true;
                mcu.triggered_timer_id = self->timer_id;
            }

            if (!mcu.running) {
                return;
            }
        }

        std::this_thread::sleep_for(TIMER_SLEEP[timer_id]);
    }
}
