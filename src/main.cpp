#include <micro16.hpp>
#include <sdl_screen.hpp>
#include <argparse.hpp>
#include <reader.hpp>

int main(int argc, char** argv)
{
    argparse::ArgumentParser arg_parser("micro16");
    arg_parser.add_argument("input_file")
        .help("Binary file to run");

    try {
        arg_parser.parse_args(argc, argv);
    } catch (const std::runtime_error& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << arg_parser;
        return -1;
    }

    auto input_file = arg_parser.get<std::string>("input_file");
    Micro16 mcu{read_code_from_file(input_file)};
    SDLScreen monitor{};

    mcu.register_mmio(monitor, Address{0x0000});
    monitor.register_on_window_close_callback([&mcu]() {
        mcu.force_halt();
    });
    auto mcu_runner = std::thread{[&mcu]() {
        mcu.run();
    }};

    while (monitor.is_connected()) {
        monitor.update();
    }
    mcu_runner.join();

    return 0;
}



