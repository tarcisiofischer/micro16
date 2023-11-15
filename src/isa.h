#ifndef MICRO16_ISA_H
#define MICRO16_ISA_H

#include <specs.h>

// Arithmetic and Logic instructions
static constexpr Byte NOP_CODE{0x00};
static constexpr Byte ADD_CODE{0x01};
static constexpr Byte SUB_CODE{0x02};
static constexpr Byte AND_CODE{0x03};
static constexpr Byte OR_CODE{0x04};
static constexpr Byte XOR_CODE{0x05};
static constexpr Byte INC_CODE{0x06};
static constexpr Byte DEC_CODE{0x07};
static constexpr Byte SET_CODE{0x08};
static constexpr Byte CLR_CODE{0x0A};
static constexpr Byte NOT_CODE{0x0B};

// Branch instructions
static constexpr Byte JMP_CODE{0x80};
static constexpr Byte BRE_CODE{0x82};
static constexpr Byte BRNE_CODE{0x83};
static constexpr Byte BRL_CODE{0x84};
static constexpr Byte BRH_CODE{0x85};
static constexpr Byte CALL_CODE{0x86};
static constexpr Byte RET_CODE{0x87};
static constexpr Byte RETI_CODE{0x88};
static constexpr Byte BRNZ_CODE{0x89};

// Memory instructions
static constexpr Byte LD_CODE{0x41};
static constexpr Byte ST_CODE{0x42};
static constexpr Byte CPY_CODE{0x43};
static constexpr Byte PUSH_CODE{0x48};
static constexpr Byte POP_CODE{0x49};
static constexpr Byte PEEK_CODE{0x4a};
static constexpr Byte CSP_CODE{0x4b};
static constexpr Byte SPXL_CODE{0x4c};

// Control instructions
static constexpr Byte DAI_CODE{0xC0};
static constexpr Byte EAI_CODE{0xC1};
static constexpr Byte DTI_CODE{0xC2};
static constexpr Byte ETI_CODE{0xC3};
static constexpr Byte SELB_CODE{0xC4};
static constexpr Byte BRK_CODE{0xFE};
static constexpr Byte HLT_CODE{0xFF};

#endif //MICRO16_ISA_H
