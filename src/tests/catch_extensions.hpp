#include <micro16.hpp>
#include <iomanip>
#include <string>

using namespace std::string_literals;

std::ostream& operator<<(std::ostream& os, Micro16::InternalState const& state)
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

void check_mcu_state(Micro16 const& mcu, Micro16::InternalState const& expected_state)
{
    CHECK(mcu.get_state() == expected_state);
}
