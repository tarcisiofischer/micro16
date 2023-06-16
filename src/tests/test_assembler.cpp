#include <tests/catch.hpp>
#include <assembler/parser.hpp>
#include <assembler/output_utils.h>
#include <sstream>

auto constexpr MICRO16_ASSEMBLER_TAG = "[micro16 assembler]";

TEST_CASE("Assembler", MICRO16_ASSEMBLER_TAG) {
    auto input_file = "test_assembler/all_instructions.m16asm";
    auto tokens = Lexer::tokens_from_file(input_file);
    auto instructions = Parser::generate_instruction_list(tokens);

    auto obtained_bin = std::stringstream{};
    dump_instructions(instructions, obtained_bin);

    using Byte = std::bitset<8>;
    auto check_next_instruction = [&obtained_bin](std::string&& opcode, Byte expected_ls, Byte expected_rs) {
        auto obtained_ls = Byte(obtained_bin.get());
        auto obtained_rs = Byte(obtained_bin.get());
        SECTION("check_next_instruction: " + opcode) {
            REQUIRE(expected_ls == obtained_ls);
            REQUIRE(expected_rs == obtained_rs);
        }
    };

    /* Arithmetic & Logic instructions */
    check_next_instruction("NOP", 0b00000000, 0b00000000);
    check_next_instruction("ADD", 0b00000001, 0b00000110);
    check_next_instruction("SUB", 0b00000010, 0b00000110);
    check_next_instruction("AND", 0b00000011, 0b00000110);
    check_next_instruction("OR",  0b00000100, 0b00000110);
    check_next_instruction("XOR", 0b00000101, 0b00000110);
    check_next_instruction("INC", 0b00000110, 0b00000011);
    check_next_instruction("DEC", 0b00000111, 0b00000011);
    check_next_instruction("SET", 0b00001000, 0b11011010);
    check_next_instruction("CLR", 0b00001010, 0b00000011);
    check_next_instruction("NOT", 0b00001011, 0b00000011);

    /* Branch instructions */
    check_next_instruction("JMP",  0b10000000, 0b00000000);
    check_next_instruction("BRE",  0b10000010, 0b00000110);
    check_next_instruction("BRNE", 0b10000011, 0b00000110);
    check_next_instruction("BRL",  0b10000100, 0b00000110);
    check_next_instruction("BRH",  0b10000101, 0b00000110);
    check_next_instruction("BRNZ", 0b10001001, 0b00000001);
    check_next_instruction("CALL", 0b10000110, 0b00000000);
    check_next_instruction("RET",  0b10000111, 0b00000000);
    check_next_instruction("RETI", 0b10001000, 0b00000000);

    /* Memory instructions */
    check_next_instruction("LD",   0b01000001, 0b00000001);
    check_next_instruction("SD",   0b01000010, 0b00000001);
    check_next_instruction("CPY",  0b01000011, 0b00000001);
    check_next_instruction("PUSH", 0b01001000, 0b00000000);
    check_next_instruction("POP",  0b01001001, 0b00000000);
    check_next_instruction("PEEK", 0b01001010, 0b00111111);

    /* Control instructions */
    check_next_instruction("DAI",  0b11000000, 0b00000000);
    check_next_instruction("EAI",  0b11000001, 0b00000000);
    check_next_instruction("DTI",  0b11000010, 0b00000001);
    check_next_instruction("ETI",  0b11000011, 0b00000001);
    check_next_instruction("SELB", 0b11000100, 0b00000011);
    check_next_instruction("BRK",  0b11111110, 0b00000000);
    check_next_instruction("HLT",  0b11111111, 0b00000000);

    /* End of program (next instruction will be all zeros, which is NOP)*/
    check_next_instruction("NOP", 0b00000000, 0b00000000);
}
