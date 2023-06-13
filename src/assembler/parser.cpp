#include <assembler/parser.hpp>
#include <cmath>

void unexpected_token(Token const& actual, TokenType expected, std::string const& extra_msg="")
{
    auto msg = "[Parser error]: Unexpected " + token_type_as_str(actual.type) + " \"" + actual.data + "\" "
        "at line " + std::to_string(actual.line) + ". "
        "Was expecting " + token_type_as_str(expected) + ".";
    if (!extra_msg.empty()) {
        msg += " (" + extra_msg + ")";
    }
    throw ParserError{msg, actual};
}

void unknown_instruction(Token const& t)
{
    auto msg = "[Parser error]: Unknown instruction named \"" + t.data + "\" at line " + std::to_string(t.line);
    throw ParserError(msg, t);
}

void not_implemented(Token const& t)
{
    auto msg = "[Parser error]: Instruction \"" + t.data + "\" is not yet implemented";
    throw ParserError(msg, t);
}

int extract_int(Token const& t, int nbits)
{
    if (t.type != TokenType::INTEGER) {
        unexpected_token(t, TokenType::INTEGER);
    }

    auto value = [&]() {
        if (t.data.substr(0, 2) == "0b") {
            return std::stoi(t.data.substr(2, t.data.size() - 2), nullptr, 2);
        } else if (t.data.substr(0, 2) == "0x") {
            return std::stoi(t.data.substr(2, t.data.size() - 2), nullptr, 16);
        }
        return std::stoi(t.data, nullptr, 10);
    }();
    if (value < 0 || value > std::pow(2, nbits)) {
        auto msg = "[Parser error]: Expected a " + std::to_string(nbits) + " value, but got " + t.data + "(" + std::to_string(value) + ")";
        throw ParserError(msg, t);
    }
    return value;
}

int extract_register(Token const& t)
{
    if (t.type != TokenType::IDENTIFIER) {
        unexpected_token(t, TokenType::IDENTIFIER, "Should be 'W0', 'W1', 'W2' or 'W3'");
    }

    if (t.data == "W0") {
        return 0;
    } else if (t.data == "W1") {
        return 1;
    } else if (t.data == "W2") {
        return 2;
    } else if (t.data == "W3") {
        return 3;
    }
    auto msg = "Unexpected register " + t.data + ". Should be 'W0', 'W1', 'W2' or 'W3'";
    throw ParserError(msg, t);
}

Instruction arithmetic_instruction(Instruction base, std::vector<Token>::const_iterator& t)
{
    t = std::next(t);
    auto cc = extract_register(*t);
    t = std::next(t);
    auto aa = extract_register(*t);
    t = std::next(t);
    auto bb = extract_register(*t);
    return base | (cc << 4) | (aa << 2) | (bb << 0);
}

Instruction unary_instruction(Instruction base, std::vector<Token>::const_iterator& t)
{
    t = std::next(t);
    auto aa = extract_register(*t);
    return base | (aa << 0);
}

std::vector<Instruction> Parser::generate_instruction_list(std::vector<Token> const& tokens)
{
    auto instructions = std::vector<Instruction>{};
    auto t = tokens.cbegin();
    while(t != tokens.cend()) {
        if (t->type == TokenType::IDENTIFIER) {
            if (t->data == "NOP") {
                instructions.push_back(0b00000000);
            } else if (t->data == "ADD") {
                instructions.push_back(arithmetic_instruction(0x0100, t));
            } else if (t->data == "SUB") {
                instructions.push_back(arithmetic_instruction(0x0200, t));
            } else if (t->data == "AND") {
                instructions.push_back(arithmetic_instruction(0x0300, t));
            } else if (t->data == "OR") {
                instructions.push_back(arithmetic_instruction(0x0400, t));
            } else if (t->data == "XOR") {
                instructions.push_back(arithmetic_instruction(0x0500, t));
            } else if (t->data == "INC") {
                instructions.push_back(unary_instruction(0x0600, t));
            } else if (t->data == "DEC") {
                instructions.push_back(unary_instruction(0x0700, t));
            } else if (t->data == "SET") {
                t = std::next(t);
                auto aa = extract_register(*t);
                t = std::next(t);
                auto yy = extract_int(*t, 2);
                t = std::next(t);
                auto xxxx = extract_int(*t, 4);
                auto instruction = 0b0000100000000000 | (aa << 6) | (yy << 4) | (xxxx << 0);
                instructions.push_back(instruction);
            } else if (t->data == "CLR") {
                t = std::next(t);
                auto aa = extract_int(*t, 2);
                auto instruction = 0b0000101000000000 | (aa << 0);
                instructions.push_back(instruction);
            } else if (t->data == "NOT") {
                t = std::next(t);
                auto aa = extract_int(*t, 2);
                auto instruction = 0b0000101100000000 | (aa << 0);
                instructions.push_back(instruction);
            } else if (t->data == "JMP") {
                t = std::next(t);
                auto aa = extract_register(*t);
                auto instruction = 0b1000000000000000 | (aa << 0);
                instructions.push_back(instruction);
            } else if (t->data == "BRE") {
                t = std::next(t);
                auto cc = extract_int(*t, 2);
                t = std::next(t);
                auto aa = extract_int(*t, 2);
                t = std::next(t);
                auto bb = extract_int(*t, 2);
                auto instruction = 0b1000001000000000 | (cc << 4) | (aa << 2) | (bb << 0);
                instructions.push_back(instruction);
            } else if (t->data == "BRNE") {
                not_implemented(*t);
            } else if (t->data == "BRL") {
                not_implemented(*t);
            } else if (t->data == "BRH") {
                not_implemented(*t);
            } else if (t->data == "BRNZ") {
                not_implemented(*t);
            } else if (t->data == "CALL") {
                not_implemented(*t);
            } else if (t->data == "RET") {
                not_implemented(*t);
            } else if (t->data == "RETI") {
                not_implemented(*t);
            } else if (t->data == "LD") {
                t = std::next(t);
                auto aa = extract_register(*t);
                t = std::next(t);
                auto bb = extract_register(*t);
                auto instruction = 0b0100000100000000 | (aa << 2) | (bb << 0);
                instructions.push_back(instruction);
            } else if (t->data == "ST") {
                t = std::next(t);
                auto aa = extract_register(*t);
                t = std::next(t);
                auto bb = extract_register(*t);
                auto instruction = 0b0100001000000000 | (aa << 2) | (bb << 0);
                instructions.push_back(instruction);
            } else if (t->data == "CPY") {
                not_implemented(*t);
            } else if (t->data == "PUSH") {
                not_implemented(*t);
            } else if (t->data == "POP") {
                not_implemented(*t);
            } else if (t->data == "PEEK") {
                not_implemented(*t);
            } else if (t->data == "DAI") {
                not_implemented(*t);
            } else if (t->data == "EAI") {
                instructions.push_back(0b1100000100000000);
            } else if (t->data == "DTI") {
                not_implemented(*t);
            } else if (t->data == "ETI") {
                t = std::next(t);
                auto a = extract_int(*t, 1);
                auto instruction = 0b1100001100000000 | (a << 0);
                instructions.push_back(instruction);
            } else if (t->data == "SELB") {
                t = std::next(t);
                auto aa = extract_int(*t, 2);
                auto instruction = 0b1100010000000000 | (aa << 0);
                instructions.push_back(instruction);
            } else if (t->data == "BRK") {
                not_implemented(*t);
            } else if (t->data == "HLT") {
                instructions.push_back(0b1111111100000000);
            } else {
                unknown_instruction(*t);
            }
        } else if (t->type == TokenType::ENDLINE) {
            /* Ignore empty lines */
        } else {
            unexpected_token(*t, TokenType::IDENTIFIER);
        }
        t = std::next(t);
    }
    return instructions;
}
