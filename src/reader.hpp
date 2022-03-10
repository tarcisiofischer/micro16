#ifndef MICRO16_READER_HPP

#include <micro16.hpp>
#include <array>

std::array<Byte, BANK_SIZE> read_code_from_file(std::string const& input_file);

#endif
