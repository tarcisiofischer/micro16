#include <assembler/parser.hpp>
#include <isa.h>
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

std::map<Position, Instruction> Parser::generate_instruction_list(std::vector<Token> const& tokens)
{
    auto pos = Position{0x0000};
    auto instructions = std::map<Position, Instruction>{};
    auto add_instruction = [&instructions, &pos](Instruction const& i) {
        instructions[pos] = i;
        pos += 2;
    };
    auto t = tokens.cbegin();
    auto next_reg = [&t]() {
        t = std::next(t);
        return extract_register(*t);
    };
    auto next_int = [&t](int size) {
        t = std::next(t);
        return extract_int(*t, size);
    };

    while(t != tokens.cend()) {
        if (t->type == TokenType::IDENTIFIER) {
            if (t->data == "NOP") {
                add_instruction((NOP_CODE << 8));
            } else if (t->data == "ADD") {
                add_instruction((ADD_CODE << 8) | (next_reg() << 4) | (next_reg() << 2) | (next_reg() << 0));
            } else if (t->data == "SUB") {
                add_instruction((SUB_CODE << 8) | (next_reg() << 4) | (next_reg() << 2) | (next_reg() << 0));
            } else if (t->data == "AND") {
                add_instruction((AND_CODE << 8) | (next_reg() << 4) | (next_reg() << 2) | (next_reg() << 0));
            } else if (t->data == "OR") {
                add_instruction((OR_CODE << 8) | (next_reg() << 4) | (next_reg() << 2) | (next_reg() << 0));
            } else if (t->data == "XOR") {
                add_instruction((XOR_CODE << 8) | (next_reg() << 4) | (next_reg() << 2) | (next_reg() << 0));
            } else if (t->data == "INC") {
                add_instruction((INC_CODE << 8) | (next_reg() << 0));
            } else if (t->data == "DEC") {
                add_instruction((DEC_CODE << 8) | (next_reg() << 0));
            } else if (t->data == "SET") {
                add_instruction((SET_CODE << 8) | (next_reg() << 6) | (next_int(2) << 4) | (next_int(4) << 0));
            } else if (t->data == "CLR") {
                add_instruction((CLR_CODE << 8) | (next_reg() << 0));
            } else if (t->data == "NOT") {
                add_instruction((NOT_CODE << 8) | (next_reg() << 0));
            } else if (t->data == "JMP") {
                add_instruction((JMP_CODE << 8) | (next_reg() << 0));
            } else if (t->data == "BRE") {
                add_instruction((BRE_CODE << 8) | (next_reg() << 4) | (next_reg() << 2) | (next_reg() << 0));
            } else if (t->data == "BRNE") {
                add_instruction((BRNE_CODE << 8) | (next_reg() << 4) | (next_reg() << 2) | (next_reg() << 0));
            } else if (t->data == "BRL") {
                add_instruction((BRL_CODE << 8) | (next_reg() << 4) | (next_reg() << 2) | (next_reg() << 0));
            } else if (t->data == "BRH") {
                add_instruction((BRH_CODE << 8) | (next_reg() << 4) | (next_reg() << 2) | (next_reg() << 0));
            } else if (t->data == "BRNZ") {
                add_instruction((BRNZ_CODE << 8) | (next_reg() << 2) | (next_reg() << 0));
            } else if (t->data == "CALL") {
                add_instruction((CALL_CODE << 8) | (next_reg() << 0));
            } else if (t->data == "RET") {
                add_instruction((RET_CODE << 8));
            } else if (t->data == "RETI") {
                add_instruction((RETI_CODE << 8));
            } else if (t->data == "LD") {
                add_instruction((LD_CODE << 8) | (next_reg() << 2) | (next_reg() << 0));
            } else if (t->data == "ST") {
                add_instruction((ST_CODE << 8) | (next_reg() << 2) | (next_reg() << 0));
            } else if (t->data == "CPY") {
                add_instruction((CPY_CODE << 8) | (next_reg() << 2) | (next_reg() << 0));
            } else if (t->data == "PUSH") {
                add_instruction((PUSH_CODE << 8) | (next_reg() << 0));
            } else if (t->data == "POP") {
                add_instruction((POP_CODE << 8) | (next_reg() << 0));
            } else if (t->data == "PEEK") {
                add_instruction((PEEK_CODE << 8) | (next_reg() << 6) | (next_int(6) << 0));
            } else if (t->data == "DAI") {
                add_instruction((DAI_CODE << 8));
            } else if (t->data == "EAI") {
                add_instruction((EAI_CODE << 8));
            } else if (t->data == "DTI") {
                add_instruction((DTI_CODE << 8) | (next_int(1) << 0));
            } else if (t->data == "ETI") {
                add_instruction((ETI_CODE << 8) | (next_int(1) << 0));
            } else if (t->data == "SELB") {
                add_instruction((SELB_CODE << 8) | (next_int(2) << 0));
            } else if (t->data == "BRK") {
                add_instruction((BRK_CODE << 8));
            } else if (t->data == "HLT") {
                add_instruction((HLT_CODE << 8));
            }

            /* Pseudo-instructions */
            else if (t->data == "SETREG") {
                auto reg = next_reg();
                auto val = next_int(16);
                add_instruction((SET_CODE << 8) | (reg << 6) | (3 << 4) | (((val & 0xf000) >> 12) << 0));
                add_instruction((SET_CODE << 8) | (reg << 6) | (2 << 4) | (((val & 0x0f00) >> 8) << 0));
                add_instruction((SET_CODE << 8) | (reg << 6) | (1 << 4) | (((val & 0x00f0) >> 4) << 0));
                add_instruction((SET_CODE << 8) | (reg << 6) | (0 << 4) | (((val & 0x000f) >> 0) << 0));
            } else if (t->data == "PUSHALL") {
                add_instruction((PUSH_CODE << 8) | (0b00 << 0));
                add_instruction((PUSH_CODE << 8) | (0b01 << 0));
                add_instruction((PUSH_CODE << 8) | (0b10 << 0));
                add_instruction((PUSH_CODE << 8) | (0b11 << 0));
            } else if (t->data == "POPALL") {
                add_instruction((POP_CODE << 8) | (0b11 << 0));
                add_instruction((POP_CODE << 8) | (0b10 << 0));
                add_instruction((POP_CODE << 8) | (0b01 << 0));
                add_instruction((POP_CODE << 8) | (0b00 << 0));
            }

            /* Unknown instructions */
            else {
                unknown_instruction(*t);
            }
        } else if (t->type == TokenType::ENDLINE) {
            /* Ignore empty lines */
        } else if (t->type == TokenType::SECTION) {
            pos = next_int(16);
        } else {
            unexpected_token(*t, TokenType::IDENTIFIER);
        }
        t = std::next(t);
    }
    return instructions;
}
