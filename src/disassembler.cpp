#include <string>
#include <vector>
#include <cstdio>

#include "disassembler.h"

std::vector<std::string> Disassembler::disassemble(const uint8_t *memory, int rom_size){
            std::vector<std::string> instructions_list;
            uint16_t instruction;
            char buf[64];
            
            for(int i = 0x200; i < (0x200 + rom_size); i += 2){
                instruction = memory[i] << 8 | memory[i + 1];
                switch(instruction & 0xF000){ 
                    case 0x0000:
                        switch(instruction & 0x00FF){
                            case 0xE0:
                                instructions_list.push_back("CLS");
                                break;

                            case 0xEE:
                                instructions_list.push_back("RET");
                                break;

                            default:
                                instructions_list.push_back("Unknown");
                                break;
                        }
                        break;

                    case 0x1000:
                        snprintf(buf, sizeof(buf), "JP 0x%03X", instruction & 0x0FFF);
                        instructions_list.push_back(buf);
                        break;

                    case 0x2000:
                        snprintf(buf, sizeof(buf), "CALL 0x%03X", instruction & 0x0FFF);
                        instructions_list.push_back(buf);
                        break;

                    case 0x3000:
                        snprintf(buf, sizeof(buf), "SE V%X, 0x%02X", (instruction >> 8) & 0x0F, instruction & 0x00FF);
                        instructions_list.push_back(buf);
                        break;

                    case 0x4000:
                        snprintf(buf, sizeof(buf), "SNE V%X, 0x%02X", (instruction >> 8) & 0x0F, instruction & 0x00FF);
                        instructions_list.push_back(buf);
                        break;

                    case 0x5000:
                        snprintf(buf, sizeof(buf), "SE V%X, V%X", (instruction >> 8) & 0x0F, (instruction >> 4) & 0x0F);
                        instructions_list.push_back(buf);
                        break;

                    case 0x6000:
                        snprintf(buf, sizeof(buf), "LD V%X, 0x%02X", (instruction >> 8) & 0x0F, instruction & 0x00FF);
                        instructions_list.push_back(buf);
                        break;

                    case 0x7000:
                        snprintf(buf, sizeof(buf), "ADD V%X, 0x%02X", (instruction >> 8) & 0x0F, instruction & 0x00FF);
                        instructions_list.push_back(buf);
                        break;

                    case 0x8000:
                        switch(instruction & 0x000F){
                            case 0x0:
                                snprintf(buf, sizeof(buf), "LD V%X, V%X", (instruction >> 8) & 0x0F, (instruction >> 4) & 0x0F);
                                instructions_list.push_back(buf);
                                break;

                            case 0x1:
                                snprintf(buf, sizeof(buf), "OR V%X, V%X", (instruction >> 8) & 0x0F, (instruction >> 4) & 0x0F);
                                instructions_list.push_back(buf);
                                break;

                            case 0x2:
                                snprintf(buf, sizeof(buf), "AND V%X, V%X", (instruction >> 8) & 0x0F, (instruction >> 4) & 0x0F);
                                instructions_list.push_back(buf);
                                break;

                            case 0x3:
                                snprintf(buf, sizeof(buf), "XOR V%X, V%X", (instruction >> 8) & 0x0F, (instruction >> 4) & 0x0F);
                                instructions_list.push_back(buf);
                                break;

                            case 0x4:
                                snprintf(buf, sizeof(buf), "ADD V%X, V%X", (instruction >> 8) & 0x0F, (instruction >> 4) & 0x0F);
                                instructions_list.push_back(buf);
                                break;

                            case 0x5:
                                snprintf(buf, sizeof(buf), "SUB V%X, V%X", (instruction >> 8) & 0x0F, (instruction >> 4) & 0x0F);
                                instructions_list.push_back(buf);
                                break;

                            case 0x6:
                                snprintf(buf, sizeof(buf), "SHR V%X, V%X", (instruction >> 8) & 0x0F, (instruction >> 4) & 0x0F);
                                instructions_list.push_back(buf);
                                break;

                            case 0x7:
                                snprintf(buf, sizeof(buf), "SUBN V%X, V%X", (instruction >> 8) & 0x0F, (instruction >> 4) & 0x0F);
                                instructions_list.push_back(buf);
                                break;

                            case 0xE:
                                snprintf(buf, sizeof(buf), "SHL V%X, V%X", (instruction >> 8) & 0x0F, (instruction >> 4) & 0x0F);
                                instructions_list.push_back(buf);
                                break;

                            default:
                                instructions_list.push_back("Unknown");
                                break;
                        }
                        break;
                    
                    case 0x9000:
                        snprintf(buf, sizeof(buf), "SNE V%X, V%X", (instruction >> 8) & 0x0F, (instruction >> 4) & 0x0F);
                        instructions_list.push_back(buf);
                        break;

                    case 0xA000:
                        snprintf(buf, sizeof(buf), "LD I, 0x%03X", instruction & 0x0FFF);
                        instructions_list.push_back(buf);
                        break;

                    case 0xB000:
                        snprintf(buf, sizeof(buf), "JP V0, 0x%03X", instruction & 0x0FFF);
                        instructions_list.push_back(buf);
                        break;

                    case 0xC000:
                        snprintf(buf, sizeof(buf), "RND V%X, 0x%02X", (instruction >> 8) & 0x0F, instruction & 0x00FF);
                        instructions_list.push_back(buf);
                        break;

                    case 0xD000:
                        snprintf(buf, sizeof(buf), "DRW V%X, V%X, 0x%X", (instruction >> 8) & 0x0F, (instruction >> 4) & 0x0F, instruction & 0x000F);
                        instructions_list.push_back(buf);
                        break;

                    case 0xE000:
                        switch(instruction & 0x00FF){
                            case 0x9E:
                                snprintf(buf, sizeof(buf), "SKP V%X", (instruction >> 8) & 0x0F);
                                instructions_list.push_back(buf);
                                break;

                            case 0xA1:
                                snprintf(buf, sizeof(buf), "SKNP V%X", (instruction >> 8) & 0x0F);
                                instructions_list.push_back(buf);
                                break;

                            default:
                                instructions_list.push_back("Unknown");
                                break;
                        }
                        break;

                    case 0xF000:
                        switch(instruction & 0x00FF){
                            case 0x07:
                                snprintf(buf, sizeof(buf), "LD V%X, DT", (instruction >> 8) & 0x0F);
                                instructions_list.push_back(buf);
                                break;

                            case 0x0A:
                                snprintf(buf, sizeof(buf), "LD V%X, K", (instruction >> 8) & 0x0F);
                                instructions_list.push_back(buf);
                                break;

                            case 0x15:
                                snprintf(buf, sizeof(buf), "LD DT, V%X", (instruction >> 8) & 0x0F);
                                instructions_list.push_back(buf);
                                break;

                            case 0x18:
                                snprintf(buf, sizeof(buf), "LD ST, V%X", (instruction >> 8) & 0x0F);
                                instructions_list.push_back(buf);
                                break;

                            case 0x1E:
                                snprintf(buf, sizeof(buf), "ADD I, V%X", (instruction >> 8) & 0x0F);
                                instructions_list.push_back(buf);
                                break;

                            case 0x29:
                                snprintf(buf, sizeof(buf), "LD F, V%X", (instruction >> 8) & 0x0F);
                                instructions_list.push_back(buf);
                                break;

                            case 0x33:
                                snprintf(buf, sizeof(buf), "LD B, V%X", (instruction >> 8) & 0x0F);
                                instructions_list.push_back(buf);
                                break;

                            case 0x55:
                                snprintf(buf, sizeof(buf), "LD [I], V%X", (instruction >> 8) & 0x0F);
                                instructions_list.push_back(buf);
                                break;

                            case 0x65:
                                snprintf(buf, sizeof(buf), "LD V%X, [I]", (instruction >> 8) & 0x0F);
                                instructions_list.push_back(buf);
                                break;
                                
                            default:
                                instructions_list.push_back("Unknown");
                                break;
                        }
                        break;
                    
                    default:
                        instructions_list.push_back("Unknown");
                        break;
                }
            }

            return instructions_list;
        }
