#ifndef MICRO16_BF_COMPILER_H
#define MICRO16_BF_COMPILER_H

#include <string>
#include <sstream>
#include <vector>

namespace bfc {

void dump_asm_repr(std::istream& source, std::ostream& ostream);

};

#endif //MICRO16_BF_COMPILER_H
