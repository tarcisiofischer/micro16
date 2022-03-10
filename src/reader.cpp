#include <reader.hpp>
#include <fstream>
#include <iterator>

std::array<Byte, BANK_SIZE> read_code_from_file(std::string const& input_file)
{
    auto file_contents = std::ifstream{input_file, std::ios::in | std::ios::binary};
    if (file_contents.fail()) {
        throw std::runtime_error("Could not open file " + input_file);
    }

    file_contents.seekg(0);
    auto code = std::array<Byte, BANK_SIZE>{};

    Byte byte = file_contents.get();
    for (int i = 0; !file_contents.eof(); ++i) {
        if (i >= BANK_SIZE) {
            throw std::runtime_error("File contents exceeds the memory size.");
        }
        code[i] = byte;
        byte = file_contents.get();
    }
    return code;
}
