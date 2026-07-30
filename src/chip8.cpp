#include <iostream>
#include <cstdint>
#include <cstring>
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
        //
        int rom_size;
        bool draw_flag;
        bool isInitialized;

        // constructor
        Chip8(){
            pc = 0x200; // pc always starts at 0x200 address
            I = 0;
            sp = 0;
            delay_timer = 0;
            sound_timer = 0;
            rom_size = 0;
            draw_flag = true;
            isInitialized = false;

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

        void reset(){
            pc = 0x200;
            I = 0;
            sp = 0;
            delay_timer = 0;
            sound_timer = 0;
            isInitialized = false;

            std::memset(V, 0, sizeof(V));
            std::memset(stack, 0, sizeof(stack));
            std::memset(display, 0, sizeof(display));
            std::memset(keyboard, 0, sizeof(keyboard));
        }

        // emulates the cpu cycle
        void cycle(){
            // fetch
            // chip8 memory store bytes, so the first byte goes to the 8 most significant bits, and the next byte on memory goes to the 8 least significant bits
            uint16_t opcode = (memory[pc] << 8 ) | memory[pc + 1];

            // decode
            switch(opcode & 0xF000){
                case 0x0000:
                    switch(opcode & 0x00FF){
                        case 0xE0: // 0x00E0: CLS - Clear the display
                            // clear the display array
                            std::memset(display, 0, sizeof(display));
                            // set draw flag
                            draw_flag = true;
                            pc += 2;
                            break;
        
                        case 0xEE: // 0x00EE: RET - Return from a subroutine
                            // point pc to the address on top of the stack then subtracts 1 from the stack pointer
                            pc = stack[sp];
                            sp--;
                            pc += 2;
                            break;

                        default:
                            pc += 2;
                            break;
                    }
                    break;

                case 0x1000: // 0x1NNN: JP addr - jump to address NNN
                    // set pc to the 12 least significant bits of the opcode
                    pc = opcode & 0x0FFF;
                    break;

                case 0x2000: // 0x2NNN: CALL addr - call subroutine at NNN
                    // increment stack pointer and add pc to the stack
                    sp++;
                    stack[sp] = pc;
                    // set pc to the address NNN
                    pc = opcode & 0x0FFF;
                    break;

                case 0x3000:{ // 0x3xkk: SE Vx, byte - skip next instruction if Vx equals kk
                    // isolate the x bits of the opcode (opcodes 12-9 bits)
                    unsigned int x = (opcode >> 8) & 0x0F;
                    // isolate the kk bits of the opcode (opcodes 8-0 bits)
                    unsigned int kk = opcode & 0x00FF;
                    // compare
                    if(V[x] == kk){
                        pc += 4;
                    }else{
                        pc += 2;
                    }
                    break;
                }

                case 0x4000:{ // 0x4xkk: SNE Vx, byte - skip next instruction if Vx not equal to kk
                    // isolate x bits
                    unsigned int x = (opcode >> 8) & 0x0F; 
                    // isolate kk bits
                    unsigned int kk = opcode & 0x00FF;
                    // compare
                    if(V[x] != kk){
                        pc += 4;
                    }else{
                        pc += 2;
                    }
                    break;
                }

                case 0x5000:{ // 0x5xy0: SE Vx, Vy - skip next instruction if Vx equals Vy
                    // isolate x bits
                    unsigned int x = (opcode >> 8) & 0x0F;
                    // isolate y bits
                    unsigned int y = (opcode >> 4) & 0x0F;
                    // compare
                    if(V[x] == V[y]){
                        pc += 4;
                    }else{
                        pc += 2;
                    }
                    break;
                }

                case 0x6000:{ // 0x6xkk: LD Vx, byte - load byte kk into register Vx
                    // isolate x bits
                    unsigned int x = (opcode >> 8) & 0x0F;
                    // isolate kk bits
                    unsigned int kk = opcode & 0x00FF;
                    // load kk into Vx
                    V[x] = kk;
                    pc += 2;
                    break;
                }

                case 0x7000:{ // 0x7xkk: ADD Vx, byte - add byte kk to register Vx
                    // isolate x bits
                    unsigned int x = (opcode >> 8) & 0x0F;
                    // isolate kk bits
                    unsigned int kk = opcode & 0x00FF;
                    // add kk to Vx
                    V[x] += kk;
                    pc += 2;
                    break;
                }

                case 0x8000: // cases 0x800, multiple instructions opcode
                    switch(opcode & 0x000F){
                        case 0x0:{ // 0x8xy0: LD Vx, Vy - stores Vy value in Vx
                            // isolate x bits
                            unsigned int x = (opcode >> 8) & 0x0F;
                            // isolate y bits
                            unsigned int y = (opcode >> 4) & 0x0F;
                            // load Vy into Vx
                            V[x] = V[y];
                            pc += 2;
                            break;
                        }

                        case 0x1:{ // 0x8xy1: set Vx = Vx OR Vy
                            // isolate x bits
                            unsigned int x = (opcode >> 8) & 0x0F;
                            // isolate y bits
                            unsigned int y = (opcode >> 4) & 0x0F;
                            // load Vx OR Vy into Vx
                            V[x] = V[x] | V[y];
                            pc += 2;
                            break;
                        }

                        case 0x2:{ // 0x8xy2: set Vx = Vx AND Vy
                            // isolate x bits
                            unsigned int x = (opcode >> 8) & 0x0F;
                            // isolate y bits
                            unsigned int y = (opcode >> 4) & 0x0F;
                            // load Vx AND Vy into Vx
                            V[x] = V[x] & V[y];
                            pc += 2;
                            break;
                        }

                        case 0x3:{ // 0x8xy3: XOR Vx, Vy - set Vx = Vx XOR Vy
                            // isolate x bits
                            unsigned int x = (opcode >> 8) & 0x0F;
                            // isolate y bits
                            unsigned int y = (opcode >> 4) & 0x0F;
                            // set Vx = Vx XOR Vy
                            V[x] = V[x] ^ V[y];
                            pc += 2;
                            break;
                        }

                        case 0x4:{ // 0x8xy4: ADD Vx, Vy - set Vx = Vx + Vy, set VF = carry
                            // isolate x bits
                            unsigned int x = (opcode >> 8) & 0x0F;
                            // isolate y bits
                            unsigned int y = (opcode >> 4) & 0x0F;
                            // Check if the sum is greater than 255 (overflow)
                            if(V[x] + V[y] > 255){
                                V[0xF] = 1;
                            }else{
                                V[0xF] = 0;
                            }
                            // add Vy to Vx
                            V[x] += V[y];
                            pc += 2;
                            break;
                        }

                        case 0x5:{ // 0x8xy5: SUB Vx, Vy - set Vx = Vx - Vy, set VF = NOT borrow
                            // isolate x bits
                            unsigned int x = (opcode >> 8) & 0x0F;
                            // isolate y bits
                            unsigned int y = (opcode >> 4) & 0x0F;
                            // set VF = NOT borrow
                            if(V[x] >= V[y]){
                                V[0xF] = 1;
                            }else{
                                V[0xF] = 0;
                            }
                            // set Vx = Vx - Vy
                            V[x] -= V[y];
                            pc += 2;
                            break;
                        }

                        case 0x6:{ // 0x8xy6: SHR Vx {, Vy} - store the least significant bit of Vx in VF and then shift Vx to the right by one bit
                            // isolate x bits
                            unsigned int x = (opcode >> 8) & 0x0F;
                            // set VF = least significant bit of Vx
                            V[0xF] = V[x] & 1;
                            // shift Vx to the right by one bit
                            V[x] >>= 1;
                            pc += 2;
                            break;
                        }
                        
                        case 0x7:{ // 0x8xy7: SUBN Vx, Vy - set Vx = Vy - Vx, set VF = NOT borrow
                            // isolate x bits
                            unsigned int x = (opcode >> 8) & 0x0F;
                            // isolate y bits
                            unsigned int y = (opcode >> 4) & 0x0F;
                            // set VF = NOT borrow
                            if(V[y] >= V[x]){
                                V[0xF] = 1;
                            }else{
                                V[0xF] = 0;
                            }
                            // set Vx = Vy - Vx
                            V[x] = V[y] - V[x];
                            pc += 2;
                            break;
                        }

                        case 0xE:{ // 0x8xyE: SHL Vx {, Vy} - store the most significant bit of Vx to VF and shift Vx to the left by one bit
                            // isolate x bits
                            unsigned int x = (opcode >> 8) & 0x0F;
                            // store the most significant bit of Vx to VF
                            V[0xF] = (V[x] >> 7) & 1;
                            // shift Vx to the left by one bit
                            V[x] <<= 1;
                            pc += 2;
                            break;
                        }

                        default:
                            pc += 2;
                            break;
                    }
                    break;
                    
                case 0x9000:{ // 0x9xy0: SNE Vx, Vy - if Vx != Vy, skip next instruction
                    // isolate x bits
                    unsigned int x = (opcode >> 8) & 0x0F;
                    // isolate y bits
                    unsigned int y = (opcode >> 4) & 0x0F;
                    // if Vx != Vy, skip next instruction
                    if(V[x] != V[y]){
                        pc += 4;
                    }else{
                        pc += 2;
                    }
                    break;
                }

                case 0xA000:{ // 0xAnnn: LD I, addr - set VI = nnn
                    // isolate nnn bits
                    unsigned int nnn = opcode & 0x0FFF;
                    // set I = nnn
                    I = nnn;
                    pc += 2;
                    break;
                }

                case 0xB000:{ // 0xBnnn: JP V0, addr - jump to addr + V0
                    // isolate nnn bits
                    unsigned int nnn = opcode & 0x0FFF;
                    // jump to addr + V0
                    pc = nnn + V[0];
                    break;
                }

                case 0xC000:{ // 0xCxkk: RND Vx, byte - set Vx = random byte AND kk
                    // isolate x bits
                    unsigned int x = (opcode >> 8) & 0x0F;
                    // isolate kk bits
                    unsigned int kk = opcode & 0x00FF;
                    // set Vx = random byte AND kk
                    V[x] = rand() & kk;
                    pc += 2;
                    break;
                }

                case 0xD000:{ // 0xDxyn: DRW Vx, Vy, nibble - display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision
                    uint8_t x = (opcode >> 8) & 0x0F;
                    uint8_t y = (opcode >> 4) & 0x0F;
                    uint8_t n = opcode & 0x000F;
                    
                    // initial coordinates
                    uint8_t xPos = V[x] % 64;
                    uint8_t yPos = V[y] % 32;
                    // reset collision flag
                    V[0xF] = 0;

                    // lines loop
                    for(unsigned row = 0; row < n; ++row){
                        // get sprite byte
                        uint8_t spriteByte = memory[I + row];
                        
                        // columns loop
                        for(unsigned col = 0; col < 8; ++col){
                            // get bit from sprite
                            uint8_t spritePixel = (0x80 >> col) & spriteByte;
                            // get x and y on screen
                            int screenX = xPos + col;
                            int screenY = yPos + row;
                            // check if within bounds
                            if(screenX >= 64 || screenY >= 32){
                                continue;
                            }

                            // calculate screen coordinate on array
                            uint32_t screenIndex = screenY * 64 + screenX;

                            // collision detection
                            // if there is a pixel in the sprite
                            if(spritePixel != 0){
                                // checks if pixel is already on
                                if(display[screenIndex] == 1){
                                    V[0xF] = 1;
                                }
                                // toggle pixel on/off
                                display[screenIndex] ^= 1;
                            }
                        }
                    }
                    draw_flag = true;
                    pc += 2;
                    break;
                }

                case 0xE000:
                    switch(opcode & 0x00FF){
                        case 0x9E:{ // 0xEx9E: SKP Vx - skip next instruction if key with value of Vx is pressed
                            uint8_t x = (opcode >> 8) & 0x0F;
                            if(keyboard[V[x]] == 1){
                                pc += 4;
                            }else{
                                pc += 2;
                            }
                            break;
                        }

                        case 0xA1:{ // 0xExA1: SKNP Vx - skip next instruction if key with value of Vx is not pressed
                            uint8_t x = (opcode >> 8) & 0x0F;
                            if(keyboard[V[x]] == 0){
                                pc += 4;
                            }else{
                                pc += 2;
                            }
                            break;
                        }

                        default:
                            pc += 2;
                            break;
                    }
                    break;

                case 0xF000:
                    switch(opcode & 0x00FF){
                        case 0x07:{ // 0xFx07: LD Vx, DT - set Vx = delay timer value
                            uint8_t x = (opcode >> 8) & 0x0F;
                            V[x] = delay_timer;
                            pc += 2;
                            break;
                        }    

                        case 0x0A:{ // 0xFx0A: LD Vx, K - store the value of the first key pressed into Vx 
                            uint8_t x = (opcode >> 8) & 0x0F;

                            // loop through all keys 
                            bool keyPressed = false;
                            for(unsigned int i = 0; i < 16; i++){
                                if(keyboard[i] == 1){
                                    V[x] = i;
                                    keyPressed = true;
                                    break;
                                }
                            }
                            // if no key is pressed, stay on same instruction
                            if(!keyPressed){
                              return; 
                            }
                            pc += 2;
                            break;
                        }

                        case 0x15:{ // 0xFx15: LD DT, Vx - set delay timer = Vx
                            uint8_t x = (opcode >> 8) & 0x0F;
                            delay_timer = V[x];
                            pc += 2;
                            break;
                        }

                        case 0x18:{ // 0xFx18: LD ST, Vx - set sound timer = Vx
                            uint8_t x = (opcode >> 8) & 0x0F;
                            sound_timer = V[x];
                            pc += 2;
                            break;
                        }

                        case 0x1E:{ // 0xFx1E: ADD I, Vx - Set I = I + Vx
                            uint8_t x = (opcode >> 8) & 0x0F;
                            I = I + V[x];
                            pc += 2;
                            break;
                        }

                        case 0x29:{ // 0xFx29: LD F, Vx - set I = location of sprite for digit Vx
                            uint8_t x = (opcode >> 8) & 0x0F;
                            I = 0x50 + (V[x] * 5);
                            pc += 2;
                            break;
                        }

                        case 0x33:{ // 0xFx33: LD B, Vx - store BCD representation of Vx in memory locations I, I+1, and I+2
                            uint8_t x = (opcode >> 8) & 0x0F;
                            // get value of Vx
                            uint8_t value = V[x];
                            // hundreds digit
                            memory[I] = value / 100;
                            // tens digit
                            memory[I + 1] = (value / 10) % 10;
                            // ones digit
                            memory[I + 2] = value % 10;
                            pc += 2;
                            break;
                        }

                        case 0x55:{ // 0xFx55: LD [I], Vx - store registers V0 through Vx in memory starting at location I
                            uint8_t x = (opcode >> 8) & 0x0F;
                            // copy values from V0 to Vx to memory starting at I
                            for(int i = 0; i <= x; i++){
                                memory[I + i] = V[i];
                            }
                            pc += 2;
                            break;
                        }

                        case 0x65:{ // 0xFx65: LD Vx, [I] - read registers V0 through Vx from memory starting at location I
                            uint8_t x = (opcode >> 8) & 0x0F;
                            // copy values from memory starting at I to V0 to Vx
                            for(int i = 0; i <= x; i++){
                                V[i] = memory[I + i];
                            }
                            pc += 2;
                            break;
                        }

                        default:
                            pc += 2;
                            break;
                    }
                    break;

                default: // unknown opcodes are ignored
                    // increments pc to the next instruction
                    pc += 2;
                    break;
            }
        }

        // loads ROM to memory
        bool loadROM(const std::string& fileName){
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
                rom_size = size;
                isInitialized = true;
                return true;
            }
            return false;
        }
};
