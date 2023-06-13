#ifndef MICRO16_PARSER_HPP
#define MICRO16_PARSER_HPP

#include <assembler/lexer.hpp>
#include <string>
#include <map>

class ParserError : public std::runtime_error {
public:
    explicit ParserError(std::string const& msg, Token token) : std::runtime_error(msg), token(token)
    {}

    Token token;
};

using Instruction = uint16_t;
using Position = unsigned long long;
class Parser {
public:
    static std::map<Position, Instruction> generate_instruction_list(std::vector<Token> const& tokens);
};

#endif //MICRO16_PARSER_HPP
