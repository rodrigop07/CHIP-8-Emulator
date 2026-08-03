#pragma once

#include <cstdint>
#include <string>
#include <vector>

class Chip8 {
public:
    // chip-8 components
    uint8_t memory[4096]; // 4kb ram
    uint8_t V[16]; // 16 8-bit registers
    uint16_t I; // index register
    uint16_t pc; // program counter
    
    uint16_t stack[16]; // stack to call subroutines
    uint16_t sp; // points to the top of the stack
    
    uint8_t delay_timer; // delay timer
    uint8_t sound_timer; // sound timer
    
    uint8_t display[64 * 32]; // 64*32px monocromatic display
    uint8_t keyboard[16]; // 16 keys keyboard
    
    int rom_size;
    bool draw_flag;
    bool isInitialized;

    // constructor
    Chip8();
    
    // loads ROM to memory
    bool loadROM(const std::string& fileName);
    
    // resets the emulator state
    void reset();

    // executes one cpu cycle
    void cycle();
};
