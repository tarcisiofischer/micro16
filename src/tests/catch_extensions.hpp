#include <micro16.hpp>
#include <iomanip>
#include <string>

using namespace std::string_literals;

void check_mcu_state(Micro16 const& mcu, Micro16::InternalState const& expected_state)
{
    CHECK(mcu.get_state() == expected_state);
}
