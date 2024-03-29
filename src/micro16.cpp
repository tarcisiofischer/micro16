#include <micro16.hpp>
#include <sstream>

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

Micro16::~Micro16()
{
    this->disconnect_adapters();
    this->running = false;
}

void Micro16::run()
{
    while (true) {
        this->check_interrupts();
        auto instruction = this->instruction_fetch();
        this->run_instruction(instruction);
        if (!this->running) {
            this->disconnect_adapters();
            break;
        }
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

void Micro16::disconnect_adapters()
{
    for (auto adapter : this->adapters) {
        if (adapter->is_connected()) {
            adapter->disconnect();
        }
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

void Micro16::set_breakpoint_handler(std::function<void()> const& handler)
{
    this->breakpoint_handler = handler;
}

Micro16::InternalState Micro16::get_state() const
{
    return {
        this->running,
        this->IP,
        this->CR,
        this->SP,
        this->W[0],
        this->W[1],
        this->W[2],
        this->W[3]
    };
}

void Micro16::force_halt()
{
    this->running = false;
}

void Micro16::run_instruction(Instruction const& instruction)
{
    auto instruction_code = static_cast<Byte>((instruction & 0xff00) >> 8);
    auto instruction_data = static_cast<Byte>(instruction & 0x00ff);

    bool IP_changed = false;

    switch (instruction_code) {
        case NOP_CODE: {
            break;
        }
        case ADD_CODE: {
            auto aa = (instruction_data & 0b00001100) >> 2;
            auto bb = (instruction_data & 0b00000011) >> 0;
            auto cc = (instruction_data & 0b00110000) >> 4;

            this->W[cc] = this->W[aa] + this->W[bb];
            break;
        }
        case SUB_CODE: {
            auto aa = (instruction_data & 0b00001100) >> 2;
            auto bb = (instruction_data & 0b00000011) >> 0;
            auto cc = (instruction_data & 0b00110000) >> 4;

            this->W[cc] = this->W[aa] - this->W[bb];
            break;
        }
        case AND_CODE: {
            auto aa = (instruction_data & 0b00001100) >> 2;
            auto bb = (instruction_data & 0b00000011) >> 0;
            auto cc = (instruction_data & 0b00110000) >> 4;

            this->W[cc] = this->W[aa] & this->W[bb];
            break;
        }
        case OR_CODE: {
            auto aa = (instruction_data & 0b00001100) >> 2;
            auto bb = (instruction_data & 0b00000011) >> 0;
            auto cc = (instruction_data & 0b00110000) >> 4;

            this->W[cc] = this->W[aa] | this->W[bb];
            break;
        }
        case XOR_CODE: {
            auto aa = (instruction_data & 0b00001100) >> 2;
            auto bb = (instruction_data & 0b00000011) >> 0;
            auto cc = (instruction_data & 0b00110000) >> 4;

            this->W[cc] = this->W[aa] ^ this->W[bb];
            break;
        }
        case INC_CODE: {
            auto aa = (instruction_data & 0b00000011) >> 0;

            this->W[aa] += 1;
            break;
        }
        case DEC_CODE: {
            auto aa = (instruction_data & 0b00000011) >> 0;

            this->W[aa] -= 1;
            break;
        }
        case SET_CODE: {
            auto aa = (instruction_data & 0b11000000) >> 6;
            auto yy = (instruction_data & 0b00110000) >> 4;
            auto xxxx = (instruction_data & 0b00001111) >> 0;

            this->W[aa] &= ~(0x000F << (4 * yy));
            this->W[aa] |= xxxx << (4 * yy);
            break;
        }
        case CLR_CODE: {
            auto aa = (instruction_data & 0b00000011) >> 0;

            this->W[aa] = 0;
            break;
        }
        case NOT_CODE: {
            auto aa = (instruction_data & 0b00000011) >> 0;

            this->W[aa] = ~this->W[aa];
            break;
        }
        case JMP_CODE: {
            auto aa = (instruction_data & 0b00000011) >> 0;

            this->IP = this->W[aa];
            IP_changed = true;
            break;
        }
        case BRE_CODE: {
            auto aa = (instruction_data & 0b00001100) >> 2;
            auto bb = (instruction_data & 0b00000011) >> 0;
            auto cc = (instruction_data & 0b00110000) >> 4;

            if (this->W[aa] == this->W[bb]) {
                this->IP = this->W[cc];
                IP_changed = true;
            }
            break;
        }
        case BRNE_CODE: {
            auto aa = (instruction_data & 0b00001100) >> 2;
            auto bb = (instruction_data & 0b00000011) >> 0;
            auto cc = (instruction_data & 0b00110000) >> 4;

            if (this->W[aa] != this->W[bb]) {
                this->IP = this->W[cc];
                IP_changed = true;
            }
            break;
        }
        case BRL_CODE: {
            auto aa = (instruction_data & 0b00001100) >> 2;
            auto bb = (instruction_data & 0b00000011) >> 0;
            auto cc = (instruction_data & 0b00110000) >> 4;

            if (this->W[aa] < this->W[bb]) {
                this->IP = W[cc];
                IP_changed = true;
            }
            break;
        }
        case BRH_CODE: {
            auto aa = (instruction_data & 0b00001100) >> 2;
            auto bb = (instruction_data & 0b00000011) >> 0;
            auto cc = (instruction_data & 0b00110000) >> 4;

            if (this->W[aa] > this->W[bb]) {
                this->IP = W[cc];
                IP_changed = true;
            }
            break;
        }
        case CALL_CODE: {
            auto stack_bank = (this->CR & 0x3000) >> 12;
            auto aa = (instruction_data & 0b00000011) >> 0;

            this->SP = this->SP + 2;
            auto next_instruction = this->IP + 2;
            this->memory_banks[stack_bank][this->SP] = (next_instruction & 0xff00) >> 8;
            this->memory_banks[stack_bank][this->SP + 1] = (next_instruction & 0x00ff) >> 0;
            this->IP = this->W[aa];

            IP_changed = true;
            break;
        }
        case BRNZ_CODE: {
            auto aa = (instruction_data & 0b00000011) >> 0;
            auto cc = (instruction_data & 0b00001100) >> 2;

            if (this->W[aa] != 0) {
                this->IP = W[cc];
                IP_changed = true;
            }
            break;
        }
        case RET_CODE: {
            auto stack_bank = (this->CR & 0x3000) >> 12;
            auto raw_data_ptr = &(this->memory_banks[stack_bank][this->SP]);

            this->IP = (*(raw_data_ptr + 0) << 8) + (*(raw_data_ptr + 1) << 0);
            this->SP = this->SP - 2;

            IP_changed = true;
            break;
        }
        case RETI_CODE: {
            auto stack_bank = (this->CR & 0x3000) >> 12;
            auto raw_data_ptr = &(this->memory_banks[stack_bank][this->SP]);

            this->IP = (*(raw_data_ptr + 0) << 8) + (*(raw_data_ptr + 1) << 0);
            this->SP = this->SP - 2;
            this->CR |= 0x0008;

            IP_changed = true;
            break;
        }
        case LD_CODE: {
            auto selected_bank = (this->CR & 0xc000) >> 14;
            auto aa = (instruction_data & 0b00001100) >> 2;
            auto bb = (instruction_data & 0b00000011) >> 0;

            auto value_ptr = &(this->memory_banks[selected_bank][this->W[aa]]);
            this->W[bb] = (*(value_ptr + 0) << 8) + (*(value_ptr + 1) << 0);
            break;
        }
        case ST_CODE: {
            auto selected_bank = (this->CR & 0xc000) >> 14;
            auto aa = (instruction_data & 0b00001100) >> 2;
            auto bb = (instruction_data & 0b00000011) >> 0;

            this->memory_banks[selected_bank][this->W[aa]] = (this->W[bb] & 0xff00) >> 8;
            this->memory_banks[selected_bank][this->W[aa] + 1] = (this->W[bb] & 0x00ff) >> 0;
            break;
        }
        case CPY_CODE: {
            auto aa = (instruction_data & 0b00001100) >> 2;
            auto bb = (instruction_data & 0b00000011) >> 0;

            this->W[bb] = this->W[aa];
            break;
        }
        case PUSH_CODE: {
            auto stack_bank = (this->CR & 0x3000) >> 12;
            auto aa = (instruction_data & 0b00000011) >> 0;

            this->SP = this->SP + 2;
            this->memory_banks[stack_bank][this->SP] = (this->W[aa] & 0xff00) >> 8;
            this->memory_banks[stack_bank][this->SP + 1] = (this->W[aa] & 0x00ff) >> 0;
            break;
        }
        case POP_CODE: {
            auto stack_bank = (this->CR & 0x3000) >> 12;
            auto raw_data_ptr = &(this->memory_banks[stack_bank][this->SP]);
            auto aa = (instruction_data & 0b00000011) >> 0;

            this->W[aa] = (*(raw_data_ptr + 0) << 8) + (*(raw_data_ptr + 1) << 0);
            this->SP = this->SP - 2;
            break;
        }
        case PEEK_CODE: {
            auto stack_bank = (this->CR & 0x3000) >> 12;
            auto raw_data_ptr = &(this->memory_banks[stack_bank][this->SP]);
            auto aa = (instruction_data & 0b11000000) >> 6;
            auto xx = (instruction_data & 0b00111111) >> 0;

            this->W[aa] = (*(raw_data_ptr - xx) << 8) + (*(raw_data_ptr - xx + 1) << 0);
            break;
        }
        case CSP_CODE: {
            auto aa = (instruction_data & 0b11000000) >> 6;
            auto xx = (instruction_data & 0b00111111) >> 0;

            this->W[aa] = this->SP - xx;
            break;
        }
        case SPXL_CODE: {
            auto aa = (instruction_data & 0b00001100) >> 2;
            auto bb = (instruction_data & 0b00000011) >> 0;

            auto video_byte = this->W[bb] / 2;
            auto side = (this->W[bb] % 2) & 0b1;
            auto nibble = this->W[aa] & 0xf;
            auto offset = side == 0 ? 4 : 0;

            this->memory_banks[0b01][video_byte] |= (nibble << offset);
            break;
        }
        case DAI_CODE: {
            this->CR &= ~(0x0008);
            break;
        }
        case EAI_CODE: {
            this->CR |= 0x0008;
            break;
        }
        case DTI_CODE: {
            auto a = (instruction_data & 0b00000001) >> 0;

            this->CR &= ~(0x0100 << a);
            break;
        }
        case ETI_CODE: {
            auto a = (instruction_data & 0b00000001) >> 0;

            this->CR |= (0x0100 << a);
            break;
        }
        case SELB_CODE: {
            auto aa = (instruction_data & 0b00000011) >> 0;

            this->CR &= 0x3fff;
            this->CR |= (aa << 14);
            break;
        }
        case BRK_CODE: {
            if (this->breakpoint_handler) {
                this->breakpoint_handler();
            }
            break;
        }
        case HLT_CODE: {
            this->running = false;
            break;
        }
        default: {
            std::stringstream ss;
            ss << "Unknown instruction code " << std::hex << int(instruction_code) << "\n";
            ss << "CPU state " << this->get_state() << "\n";
            throw std::runtime_error(ss.str());
        }
    }

    if (!IP_changed) {
        this->IP += 2;
    }
    if (this->IP >= BANK_SIZE) {
        this->IP = 0;
    }

    // Assert all registers are 16bit
    for (int i = 0; i < 4; ++i) {
        this->W[i] &= 0x0000ffff;
    }
    this->IP &= 0x0000ffff;
    this->CR &= 0x0000ffff;
    this->SP &= 0x0000ffff;

}

Micro16::TimerInterruptHandler::TimerInterruptHandler(Micro16& mcu, int timer_id)
    : mcu{mcu}
    , timer_id{timer_id}
    , thread{runner, this}
{
}

Micro16::TimerInterruptHandler::~TimerInterruptHandler()
{
    this->thread.join();
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
