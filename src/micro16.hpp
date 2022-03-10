#ifndef MICRO16_MICRO16_H
#define MICRO16_MICRO16_H

#include <array>
#include <bitset>
#include <mutex>
#include <thread>
#include <chrono>
#include <vector>
#include <functional>

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

// Arithmetic and Logic instructions
static constexpr Byte NOP_CODE{0x00};
static constexpr Byte ADD_CODE{0x01};
static constexpr Byte SUB_CODE{0x02};
static constexpr Byte AND_CODE{0x03};
static constexpr Byte OR_CODE{0x04};
static constexpr Byte XOR_CODE{0x05};
static constexpr Byte INC_CODE{0x06};
static constexpr Byte DEC_CODE{0x07};
static constexpr Byte SET_CODE{0x08};
static constexpr Byte CLR_CODE{0x0A};
static constexpr Byte NOT_CODE{0x0B};

// Branch instructions
static constexpr Byte JMP_CODE{0x80};
static constexpr Byte BRE_CODE{0x82};
static constexpr Byte BRNE_CODE{0x83};
static constexpr Byte BRL_CODE{0x84};
static constexpr Byte BRH_CODE{0x85};
static constexpr Byte CALL_CODE{0x86};
static constexpr Byte RET_CODE{0x87};
static constexpr Byte RETI_CODE{0x88};
static constexpr Byte BRNZ_CODE{0x89};

// Memory instructions
static constexpr Byte LD_CODE{0x41};
static constexpr Byte ST_CODE{0x42};
static constexpr Byte CPY_CODE{0x43};
static constexpr Byte PUSH_CODE{0x48};
static constexpr Byte POP_CODE{0x49};
static constexpr Byte PEEK_CODE{0x4a};

// Control instructions
static constexpr Byte DAI_CODE{0xC0};
static constexpr Byte EAI_CODE{0xC1};
static constexpr Byte DTI_CODE{0xC2};
static constexpr Byte ETI_CODE{0xC3};
static constexpr Byte SELB_CODE{0xC4};
static constexpr Byte BRK_CODE{0xFE};
static constexpr Byte HLT_CODE{0xFF};

using namespace std::chrono_literals;

class Micro16 {
public:
    class Adapter {
    public:
        virtual void connect_to_memory(Byte* memory_start) = 0;
        virtual void disconnect() = 0;
    };

    class TimerInterruptHandler {
    public:
        static constexpr std::array<std::chrono::milliseconds, 2> TIMER_SLEEP = {50ms, 500ms};

        TimerInterruptHandler(Micro16& mcu, int timer_id);
        ~TimerInterruptHandler();
        static void runner(TimerInterruptHandler* self);

    private:
        Micro16& mcu;
        int timer_id;
        std::thread thread;
    };

    struct InternalState {
        bool running;
        Register IP;
        Register CR;
        Register SP;
        Register W0;
        Register W1;
        Register W2;
        Register W3;

        inline bool operator==(InternalState const& other) const {
            return (
                this->running == other.running &&
                this->IP == other.IP &&
                this->CR == other.CR &&
                this->SP == other.SP &&
                this->W0 == other.W0 &&
                this->W1 == other.W1 &&
                this->W2 == other.W2 &&
                this->W3 == other.W3
            );
        }
    };
public:
    Micro16(std::array<Byte, BANK_SIZE> const& code);
    ~Micro16();

    void run();
    void register_mmio(Adapter& adapter, Address request_addr);
    void set_breakpoint_handler(std::function<void()> const& handler);
    InternalState get_state() const;

private:
    Instruction instruction_fetch() const;
    void run_instruction(Instruction const& instruction);
    void check_interrupts();

private:
    bool running;

    Register IP;
    Register CR;
    Register SP;
    std::array<Register, 4> W;

    std::array<std::array<Byte, BANK_SIZE>, N_BANKS> memory_banks;

    std::mutex timer_mutex;
    bool timer_triggered;
    int triggered_timer_id;
    TimerInterruptHandler timer0;
    TimerInterruptHandler timer1;

    std::vector<Adapter*> adapters;
    std::function<void()> breakpoint_handler;
};

#endif //MICRO16_MICRO16_H
