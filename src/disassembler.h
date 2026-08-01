#pragma once

#include <string>
#include <vector>
#include <cstdint>

class Disassembler {
public:
    std::vector<std::string> disassemble(const uint8_t *memory, int rom_size);
};
