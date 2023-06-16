#ifndef MICRO16_OUTPUT_UTILS_H
#define MICRO16_OUTPUT_UTILS_H

#include <assembler/parser.hpp>
#include <micro16.hpp>
#include <array>

inline void dump_instructions(
    std::map<Position, Instruction> const& instructions,
    std::ostream& output_stream
)
{
    auto memory = std::array<char, BANK_SIZE>{};
    for (auto&& [pos, i] : instructions) {
        memory[pos + 0] = (i & 0xff00) >> 8;
        memory[pos + 1] = (i & 0x00ff) >> 0;
    }
    output_stream.write(memory.data(), memory.size());
}

#endif //MICRO16_OUTPUT_UTILS_H
