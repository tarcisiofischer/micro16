#include <argparse.hpp>
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
            std::cout << "In file " << output_file << ":\n";
            std::cout << "|  " << err.what() << "\n";
            auto f = std::ifstream(input_file);
            auto s = std::string{};
            for (int i = 1; i <= err.token.line; i++) {
                std::getline(f, s);
            }
            std::cout << "|  " << err.token.line << ": " << s << "\n";
            std::cout << "|  ";
            for (int i = 0; i < err.token.col; ++i) {
                std::cout << ".";
            }
            std::cout << "^\n";
        }
        return {};
    }();
    for (auto&& [pos, i] : instructions) {
        std::cout << std::hex << pos << ": " << std::bitset<16>(i) << "\n";
    }

    return 0;
}



