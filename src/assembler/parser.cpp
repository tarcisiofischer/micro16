#include <assembler/parser.hpp>
#include <iostream>

void unexpected_token(Token const& t)
{
    throw std::runtime_error("Unexpected token " + token_type_as_str(t.type) + " at line " + std::to_string(t.line));
}

void unknown_instruction(Token const& t)
{
    throw std::runtime_error("Unknown instruction named \"" + t.data + "\" at line " + std::to_string(t.line));
}

void not_implemented(Token const& t)
{
    throw std::runtime_error("Instruction \"" + t.data + "\" is not yet implemented");
}

int extract_int(Token const& t, int nbits)
{
    if (t.type != TokenType::INTEGER) {
        unexpected_token(t);
    }

    // TODO: Extract the integer
    return 0;
}

int extract_register(Token const& t)
{
    if (t.type != TokenType::IDENTIFIER) {
        unexpected_token(t);
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
    throw std::runtime_error("Unexpected register " + t.data + ". Should be 'W0', 'W1', 'W2' or 'W3'");
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
        std::cout << ".> " << t->data << "\n";
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
                not_implemented(*t);
            } else if (t->data == "CLR") {
                not_implemented(*t);
            } else if (t->data == "NOT") {
                not_implemented(*t);
            } else if (t->data == "JMP") {
                not_implemented(*t);
            } else if (t->data == "BRE") {
                not_implemented(*t);
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
                not_implemented(*t);
            } else if (t->data == "ST") {
                not_implemented(*t);
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
                not_implemented(*t);
            } else if (t->data == "DTI") {
                not_implemented(*t);
            } else if (t->data == "ETI") {
                not_implemented(*t);
            } else if (t->data == "SELB") {
                t = std::next(t);
                auto aa = extract_int(*t, 2);
                auto instruction = 0b1100010000000000 | (aa << 0);
                instructions.push_back(instruction);
            } else if (t->data == "BRK") {
                not_implemented(*t);
            } else if (t->data == "HLT") {
                instructions.push_back(0xFF00);
            } else {
                unknown_instruction(*t);
            }
        } else if (t->type == TokenType::ENDLINE) {
            /* Ignore empty lines */
        } else {
            unexpected_token(*t);
        }
        t = std::next(t);
    }
    return instructions;
}
