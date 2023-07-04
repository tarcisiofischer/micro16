#ifndef MICRO16_MICRO16_H
#define MICRO16_MICRO16_H

#include <specs.h>
#include <isa.h>
#include <array>
#include <bitset>
#include <mutex>
#include <thread>
#include <chrono>
#include <vector>
#include <functional>
#include <iomanip>

using namespace std::string_literals;
using namespace std::chrono_literals;

class Micro16 {
public:
    class Adapter {
    public:
        virtual void connect_to_memory(Byte* memory_start) = 0;
        virtual bool is_connected() const = 0;
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
    void force_halt();

private:
    Instruction instruction_fetch() const;
    void run_instruction(Instruction const& instruction);
    void check_interrupts();
    void disconnect_adapters();

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

inline std::ostream& operator<<(std::ostream& os, Micro16::InternalState const& state)
{
    os << "{\n"s;
    os << "\tis_running? = "s << (state.running ? "true"s : "false"s) << "\n";
    os << "\tIP = 0x"s << std::setw(4) << std::setfill('0') << std::hex << state.IP << "\n";
    os << "\tCR = 0x"s << std::setw(4) << std::setfill('0') << std::hex << state.CR << "\n";
    os << "\tSP = 0x"s << std::setw(4) << std::setfill('0') << std::hex << state.SP << "\n";
    os << "\tW0 = 0x"s << std::setw(4) << std::setfill('0') << std::hex << state.W0 << "\n";
    os << "\tW1 = 0x"s << std::setw(4) << std::setfill('0') << std::hex << state.W1 << "\n";
    os << "\tW2 = 0x"s << std::setw(4) << std::setfill('0') << std::hex << state.W2 << "\n";
    os << "\tW3 = 0x"s << std::setw(4) << std::setfill('0') << std::hex << state.W3 << "\n";
    os << "}"s;
    return os;
}

#endif //MICRO16_MICRO16_H
