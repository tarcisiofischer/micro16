#include <brainfuck/compiler.hpp>

#include <argparse.hpp>
#include <fstream>

int main(int argc, char** argv)
{
    argparse::ArgumentParser arg_parser("micro16_bfc");
    arg_parser.add_argument("input_file")
            .help("Brainfuck source (.bfc)");
    arg_parser.add_argument("output_file")
            .help("Output (.m16asm)");

    try {
        arg_parser.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << arg_parser;
        return -1;
    }

    auto input_file = arg_parser.get<std::string>("input_file");
    auto output_file = arg_parser.get<std::string>("output_file");

    auto file_contents = std::ifstream{input_file, std::ios::in};
    if (file_contents.fail()) {
        throw std::runtime_error("Could not open file " + input_file);
    }

    auto out_stream = std::ofstream{output_file, std::ios::out | std::ios::binary};
    bfc::dump_asm_repr(file_contents, out_stream);

    return 0;
}
