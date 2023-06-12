#ifndef MICRO16_PARSER_HPP
#define MICRO16_PARSER_HPP

#include <assembler/lexer.hpp>
#include <vector>

using Instruction = uint16_t;
class Parser {
public:
    static std::vector<Instruction> generate_instruction_list(std::vector<Token> const& tokens);
};

#endif //MICRO16_PARSER_HPP
