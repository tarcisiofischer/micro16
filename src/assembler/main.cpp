#include <argparse.hpp>
#include <assembler/lexer.hpp>

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
    for (auto&& t : tokens) {
        std::cout << static_cast<int>(t.type) << " : " << t.data << " (" << t.line << ", " << t.col << ")\n";
    }

    return 0;
}


