#include <argparse.hpp>
#include <micro16.hpp>
#include <assembler/lexer.hpp>
#include <assembler/parser.hpp>
#include <bitset>

int main(int argc, char** argv)
{
    argparse::ArgumentParser arg_parser("micro16");
    arg_parser.add_argument("input_file")
        .help("Micro16 ASM file (.m16asm)");
    arg_parser.add_argument("output_file")
        .help("Output binary (.micro16)");

    try {
        arg_parser.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << arg_parser;
        return -1;
    }

    auto input_file = arg_parser.get<std::string>("input_file");
    auto output_file = arg_parser.get<std::string>("output_file");

    auto tokens = Lexer::tokens_from_file(input_file);
    auto instructions = [&]() -> decltype(Parser::generate_instruction_list(tokens)) {
        try {
            return Parser::generate_instruction_list(tokens);
        } catch (ParserError const& err) {
            std::cerr << "In file " << output_file << ":\n";
            std::cerr << "|  " << err.what() << "\n";
            auto f = std::ifstream(input_file);
            auto s = std::string{};
            for (int i = 1; i <= err.token.line; i++) {
                std::getline(f, s);
            }
            std::cerr << "|  " << err.token.line << ": " << s << "\n";
            std::cerr << "|  ";
            for (int i = 0; i < err.token.col; ++i) {
                std::cerr << ".";
            }
            std::cerr << "^\n";
        }
        return {};
    }();
    if (instructions.empty()) {
        std::cerr << "Could not generate code.\n";
        return -1;
    }

    auto memory = std::array<char, BANK_SIZE>{};
    for (auto&& [pos, i] : instructions) {
        memory[pos + 0] = (i & 0xff00) >> 8;
        memory[pos + 1] = (i & 0x00ff) >> 0;
    }
    auto out_file = std::ofstream{output_file, std::ios::out | std::ios::binary};
    auto* d = memory.data();
    out_file.write(d, memory.size());

    return 0;
}



