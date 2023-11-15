#include <tests/catch.hpp>
#include <tests/catch_extensions.hpp>
#include <micro16.hpp>

auto constexpr MICRO16_INSTRUCTIONS_TAG = "[micro16 instructions]";

TEST_CASE("Basic arithmetic instructions", MICRO16_INSTRUCTIONS_TAG) {
    auto code = std::array<Byte, BANK_SIZE>{
/*0x0000*/    SET_CODE, 0b00001010,
/*0x0002*/    SET_CODE, 0b01000010,
/*0x0004*/    ADD_CODE, 0b00110001,
/*0x0006*/    BRK_CODE, 0b00000000,
/*0x0008*/    SUB_CODE, 0b00110001,
/*0x000a*/    BRK_CODE, 0b00000000,
/*0x000c*/    AND_CODE, 0b00110001,
/*0x000e*/    BRK_CODE, 0b00000000,
/*0x0010*/    OR_CODE, 0b00110001,
/*0x0012*/    BRK_CODE, 0b00000000,
/*0x0014*/    XOR_CODE, 0b00110001,
/*0x0016*/    BRK_CODE, 0b00000000,
/*0x0018*/    INC_CODE, 0b00000011,
/*0x001a*/    BRK_CODE, 0b00000000,
/*0x001c*/    DEC_CODE, 0b00000011,
/*0x001e*/    BRK_CODE, 0b00000000,
/*0x0020*/    NOT_CODE, 0b00000000,
/*0x0022*/    BRK_CODE, 0b00000000,
/*0x0024*/    CLR_CODE, 0b00110001,
/*0x0026*/    BRK_CODE, 0b00000000,
/*0x0028*/    HLT_CODE, 0b00000000
    };

    Micro16 mcu{code};
    mcu.set_breakpoint_handler([&]() {
        auto IP = mcu.get_state().IP;
        if (IP == 0x0006) {
            check_mcu_state(mcu, {
                true,
                IP,
                0x9000,
                0x8000,
                0x000a,
                0x0002,
                0x0000,
                0x000c
            });
        } else if (IP == 0x000a) {
            check_mcu_state(mcu, {
                true,
                IP,
                0x9000,
                0x8000,
                0x000a,
                0x0002,
                0x0000,
                0x0008
            });
        } else if (IP == 0x000e) {
            check_mcu_state(mcu, {
                true,
                IP,
                0x9000,
                0x8000,
                0x000a,
                0x0002,
                0x0000,
                0x0002
            });
        } else if (IP == 0x0012) {
            check_mcu_state(mcu, {
                true,
                IP,
                0x9000,
                0x8000,
                0x000a,
                0x0002,
                0x0000,
                0x000a
            });
        } else if (IP == 0x0016) {
            check_mcu_state(mcu, {
                true,
                IP,
                0x9000,
                0x8000,
                0x000a,
                0x0002,
                0x0000,
                0x0008
            });
        } else if (IP == 0x001a) {
            check_mcu_state(mcu, {
                true,
                IP,
                0x9000,
                0x8000,
                0x000a,
                0x0002,
                0x0000,
                0x0009
            });
        } else if (IP == 0x001e) {
            check_mcu_state(mcu, {
                true,
                IP,
                0x9000,
                0x8000,
                0x000a,
                0x0002,
                0x0000,
                0x0008
            });
        } else if (IP == 0x0022) {
            check_mcu_state(mcu, {
                true,
                IP,
                0x9000,
                0x8000,
                0xfff5,
                0x0002,
                0x0000,
                0x0008
            });
        } else if (IP == 0x0026) {
            check_mcu_state(mcu, {
                true,
                IP,
                0x9000,
                0x8000,
                0xfff5,
                0x0000,
                0x0000,
                0x0008
            });
        } else {
            FAIL("Unhandled breakpoint");
        }
    });
    mcu.run();
    REQUIRE(mcu.get_state().running == false);
    REQUIRE(mcu.get_state().IP == 0x002a);
}

TEST_CASE("JMP instruction", MICRO16_INSTRUCTIONS_TAG) {
    auto code = std::array<Byte, BANK_SIZE>{
/*0x0000*/    SET_CODE, 0b00110000,
/*0x0002*/    SET_CODE, 0b00100000,
/*0x0004*/    SET_CODE, 0b00010001,
/*0x0006*/    SET_CODE, 0b00000010,
/*0x0008*/    JMP_CODE, 0b00000000,
/*0x000a*/    SET_CODE, 0b00111111,
/*0x000c*/    SET_CODE, 0b00101111,
/*0x000e*/    SET_CODE, 0b00011111,
/*0x0010*/    SET_CODE, 0b00001111,
/*0x0012*/    BRK_CODE, 0b00000000,
/*0x0014*/    HLT_CODE, 0b00000000
    };

    Micro16 mcu{code};
    mcu.set_breakpoint_handler([&]() {
        auto IP = mcu.get_state().IP;
        if (IP == 0x0012) {
            check_mcu_state(mcu, {
                true,
                IP,
                0x9000,
                0x8000,
                0x0012,
                0x0000,
                0x0000,
                0x0000
            });
        } else {
            FAIL("Unhandled breakpoint");
        }
    });
    mcu.run();
    REQUIRE(mcu.get_state().running == false);
    REQUIRE(mcu.get_state().IP == 0x0016);
}

TEST_CASE("Memory instructions", MICRO16_INSTRUCTIONS_TAG) {
    auto code = std::array<Byte, BANK_SIZE>{
/*0x0000*/    SELB_CODE, 0b00000001,
/*0x0002*/    SET_CODE, 0b00111000,
/*0x0004*/    SET_CODE, 0b00100100,
/*0x0006*/    SET_CODE, 0b00010010,
/*0x0008*/    SET_CODE, 0b00000001,
/*0x000a*/    ST_CODE, 0b00000000,
/*0x000c*/    LD_CODE, 0b00000001,
/*0x000e*/    BRK_CODE, 0b00000000,
/*0x0010*/    SELB_CODE, 0b00000011,
/*0x0012*/    LD_CODE, 0b00000001,
/*0x0014*/    BRK_CODE, 0b00000000,
/*0x0016*/    HLT_CODE, 0b00000000
    };

    Micro16 mcu{code};
    mcu.set_breakpoint_handler([&]() {
        auto IP = mcu.get_state().IP;
        if (IP == 0x000e) {
            check_mcu_state(mcu, {
                true,
                IP,
                0x5000,
                0x8000,
                0x8421,
                0x8421,
                0x0000,
                0x0000
            });
        } else if (IP == 0x0014) {
            check_mcu_state(mcu, {
                true,
                IP,
                0xd000,
                0x8000,
                0x8421,
                0x0000,
                0x0000,
                0x0000
            });
        } else {
            FAIL("Unhandled breakpoint");
        }
    });
    mcu.run();
    REQUIRE(mcu.get_state().running == false);
    REQUIRE(mcu.get_state().IP == 0x0018);
}

TEST_CASE("Stack operations", MICRO16_INSTRUCTIONS_TAG) {
    auto code = std::array<Byte, BANK_SIZE>{
/*0x0000*/    SELB_CODE, 0b00000001,
/*0x0002*/    SET_CODE, 0b00110000,
/*0x0004*/    SET_CODE, 0b00100000,
/*0x0006*/    SET_CODE, 0b00010001,
/*0x0008*/    SET_CODE, 0b00000010,
/*0x000a*/    PUSH_CODE, 0b00000000,
/*0x000c*/    CALL_CODE, 0b00000000,
/*0x000e*/    POP_CODE, 0b00000011,
/*0x0010*/    HLT_CODE, 0b00000000,
/*0x0012*/    CLR_CODE, 0b00000000,
/*0x0014*/    PEEK_CODE, 0b01000100,
/*0x0016*/    BRK_CODE, 0b00000000,
/*0x0018*/    RET_CODE, 0b00000000
    };

    Micro16 mcu{code};
    mcu.set_breakpoint_handler([&]() {
        auto IP = mcu.get_state().IP;
        if (IP == 0x0016) {
            check_mcu_state(mcu, {
                true,
                IP,
                0x5000,
                0x8004,
                0x0000,
                0x0000,
                0x0000,
                0x0000
            });
        } else {
            FAIL("Unhandled breakpoint");
        }
    });
    mcu.run();
    check_mcu_state(mcu, {
        false,
        0x0012,
        0x5000,
        0x8000,
        0x0000,
        0x0000,
        0x0000,
        0x0012
    });
}

TEST_CASE("Push/Pop from stack", MICRO16_INSTRUCTIONS_TAG) {
    auto code = std::array<Byte, BANK_SIZE>{
/*0x0000*/    SELB_CODE, 0b00000001,
/*0x0002*/    SET_CODE, 0b00001010,
/*0x0004*/    SET_CODE, 0b01000101,
/*0x0006*/    PUSH_CODE, 0b00000000,
/*0x0008*/    PUSH_CODE, 0b00000001,
/*0x000a*/    CSP_CODE, 0b11000010,
/*0x000c*/    BRK_CODE, 0b00000000,
/*0x000e*/    POP_CODE, 0b00000010,
/*0x0010*/    POP_CODE, 0b00000011,
/*0x0012*/    BRK_CODE, 0b00000000,
/*0x0014*/    HLT_CODE, 0b00000000,
    };

    Micro16 mcu{code};
    mcu.set_breakpoint_handler([&]() {
        auto IP = mcu.get_state().IP;
        if (IP == 0x000c) {
            check_mcu_state(mcu, {
                true,
                IP,
                0x5000,
                0x8004,
                0x000a,
                0x0005,
                0x0000,
                0x8002
            });
        } else if (IP == 0x0012) {
            check_mcu_state(mcu, {
                true,
                IP,
                0x5000,
                0x8000,
                0x000a,
                0x0005,
                0x0005,
                0x000a
            });
        } else {
            FAIL("Unhandled breakpoint");
        }
    });
    mcu.run();
    check_mcu_state(mcu, {
        false,
        0x0016,
        0x5000,
        0x8000,
        0x000a,
        0x0005,
        0x0005,
        0x000a
    });
}

class InspectVideoAdapter : public Micro16::Adapter
{
public:
    void connect_to_memory(Byte* memory_start) { this->video_memory_ptr = memory_start; }
    bool is_connected() const { return this->video_memory_ptr != nullptr; }
    void disconnect() { this->video_memory_ptr = nullptr; }
    Byte get_byte(int index) { return this->video_memory_ptr[index]; }

private:
    Byte* video_memory_ptr;
};

TEST_CASE("Set pixel", MICRO16_INSTRUCTIONS_TAG) {
    auto code = std::array<Byte, BANK_SIZE>{
/*0x0000*/    SET_CODE,  0b00001111,
/*0x0002*/    SET_CODE,  0b01000000,
/*0x0004*/    SPXL_CODE, 0b00000001,
/*0x0006*/    BRK_CODE,  0b00000000,

/*0x0008*/    SET_CODE,  0b00001010,
/*0x000a*/    SET_CODE,  0b01000001,
/*0x000c*/    SPXL_CODE, 0b00000001,
/*0x000e*/    BRK_CODE,  0b00000000,

/*0x0010*/    HLT_CODE,  0b00000000,
    };

    Micro16 mcu{code};
    InspectVideoAdapter adapter;
    mcu.register_mmio(adapter, Address{0x0000});
    REQUIRE(adapter.get_byte(0) == 0x00);
    mcu.set_breakpoint_handler([&]() {
        auto IP = mcu.get_state().IP;
        if (IP == 0x0006) {
            REQUIRE(adapter.get_byte(0) == 0xf0);
        } else if (IP == 0x000e) {
            REQUIRE(adapter.get_byte(0) == 0xfa);
        } else {
            FAIL("Unhandled breakpoint at " + std::to_string(IP));
        }
    });
    mcu.run();
    REQUIRE(mcu.get_state().running == false);
    REQUIRE(mcu.get_state().IP == 0x0012);
}
