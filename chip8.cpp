#include <iostream>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <vector>


class Chip8{
    public:
        // chip-8 components
        uint8_t memory[4096]; // 4kb ram
        uint8_t V[16]; // 16 8-bit registers
        uint16_t I; // index register
        uint16_t pc; // program counter
        //
        uint16_t stack[16]; // stack to call subroutines
        uint16_t sp; // points to the top of the stack
        //
        uint8_t delay_timer; // delay timer
        uint8_t sound_timer; // sound timer
        // 
        uint8_t display[64 * 32]; // 64*32px monocromatic display
        uint8_t keyboard[16]; // 16 keys keyboard

        // constructor
        Chip8(){
            pc = 0x200; // pc always starts at 0x200 address
            I = 0;
            sp = 0;
            delay_timer = 0;
            sound_timer = 0;

            // clear memory and arrays
            std::memset(memory, 0, sizeof(memory));
            std::memset(V, 0, sizeof(V));
            std::memset(stack, 0, sizeof(stack));
            std::memset(display, 0, sizeof(display));
            std::memset(keyboard, 0, sizeof(keyboard));

            // chip-8 native font
            uint8_t fontset[80] = {
                0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
                0x20, 0x60, 0x20, 0x20, 0x70, // 1
                0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
                0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
                0x90, 0x90, 0xF0, 0x10, 0x10, // 4
                0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
                0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
                0xF0, 0x10, 0x20, 0x40, 0x40, // 7
                0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
                0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
                0xF0, 0x90, 0xF0, 0x90, 0x90, // A
                0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
                0xF0, 0x80, 0x80, 0x80, 0xF0, // C
                0xE0, 0x90, 0x90, 0x90, 0xE0, // D
                0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
                0xF0, 0x80, 0xF0, 0x80, 0x80  // F
            };

            // loads font to memory from 0x50 to 0x9F
            for(int i = 0; i < 80; ++i){
                memory[0x50 + i] = fontset[i];
            }
        }

        // emulates the cpu cycle
        void cycle(){
            // fetch
            // chip8 memory store bytes, so the first byte goes to the 8 most significant bits, and the next byte on memory goes to the 8 least significant bits
            uint16_t opcode = (memory[pc] << 8 ) | memory[pc + 1];

            // decode
            switch(opcode & 0xF000){
                default: // unknown opcodes are ignored
                    // increments pc to the next instruction
                    pc += 2;
            }
        }

        // loads ROM to memory
        bool loadROM(const char* fileName){
            // opens file in binary mode and place a pointer at the end
            std::ifstream file(fileName, std::ios::binary | std::ios::ate);
            if(file.is_open()){
                std::streamsize size = file.tellg();
                file.seekg(0, std::ios::beg);

                // put file content in a buffer
                std::vector<char> buffer(size);
                if(file.read(buffer.data(), size)){
                    // copy buffer to memory from 0x200
                    for(long i = 0; i < size; ++i){
                        memory[0x200 + i] = buffer[i];
                    }
                }
                file.close();
                return true;
            }
            return false;
        }
};