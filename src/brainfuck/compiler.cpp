#include <brainfuck/compiler.hpp>

namespace bfc {

void dump_asm_repr(std::istream& source, std::ostream &ostream)
{
    auto label_n = 0;
    auto generate_next_label = [&label_n]() { label_n++; return "_" + std::to_string(label_n); };
    auto label_stack = std::vector<std::string>{};
    auto emit = [&ostream](std::string const& instruction) { ostream << instruction << "\n"; };

    source.seekg(0);
    for (auto i = 0; !source.eof(); ++i) {
        auto c = static_cast<char>(source.get());
        switch (c) {
            case '+': {
                emit("INC W3");
                break;
            }
            case '-': {
                emit("DEC W3");
                break;
            }
            case '<': {
                emit("ST W0 W3");
                emit("DEC W0");
                emit("DEC W0");
                emit("LD W0 W3");
                break;
            }
            case '>': {
                emit("ST W0 W3");
                emit("INC W0");
                emit("INC W0");
                emit("LD W0 W3");
                break;
            }
            case '[': {
                auto label = generate_next_label();
                label_stack.push_back(label);
                emit(".label " + label);
                break;
            }
            case ']': {
                if (label_stack.empty()) {
                    throw std::runtime_error("Unexpected ']' without a matching '['");
                }
                auto label = label_stack.back();
                label_stack.pop_back();
                emit("SETREG W1 " + label);
                emit("BRNZ W1 W3");
                break;
            }
            case ',': {
                // Ignored.
                break;
            }
            case '.': {
                emit("SPXL W3 W2");
                emit("INC W2");
                break;
            }
            default: {
                // Ignore any other character
            }
        }
    }
    if (!label_stack.empty()) {
        throw std::runtime_error("Unexpected EOF with a '[' without a matching ']'");
    }
}

}
