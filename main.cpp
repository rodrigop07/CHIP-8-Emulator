#include "chip8.cpp"
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <SDL2/SDL.h>

namespace fs = std::filesystem;

void chooseROM(Chip8& chip8){
    std::vector<std::string> roms;
    for(const auto &rom: fs::directory_iterator("./roms")){
        if(fs::is_regular_file(rom.status())){
            roms.push_back(rom.path().filename().string());
        }
    }
    int opc = 0;
    for(const auto &rom: roms){
        std::cout << opc++ << " - " << rom << std::endl;
    }
    std::cout << "Select a ROM: ";
    std::cin >> opc;
    while(opc < 0 || opc >= (int)roms.size()){
        std::cout << "Select a valid ROM: ";
        std::cin >> opc;
    }

    chip8.reset();
    chip8.loadROM(std::string("roms/" + roms[opc]));
}

int main(int argc, char* args[]){
    // initialize chip-8 emulator
    Chip8 chip8;
    chooseROM(chip8);

    // try to init video
    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cerr << "ERROR: " << SDL_GetError() << std::endl;
        return -1;
    }

    // create sdl window
    SDL_Window* window = SDL_CreateWindow(
        "Chip-8 Emulator", // title
        SDL_WINDOWPOS_CENTERED, // x position
        SDL_WINDOWPOS_CENTERED, // y position
        960, // width
        480, // height
        SDL_WINDOW_SHOWN
    );

    if(window == nullptr){
        std::cerr << "ERROR: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // init renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if(renderer == nullptr){
        std::cerr << "ERROR: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // create sdl texture
    SDL_Texture* texture = SDL_CreateTexture(
        renderer, 
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        64,
        32
    );

    if(texture == nullptr){
        std::cerr << "ERROR: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // create buffer to copy the display array
    uint32_t pixels[64 * 32];
    // main event loop
    bool isRunning = true;
    SDL_Event event;

    while(isRunning){
        while(SDL_PollEvent(&event) != 0){
            // check if the event is a quit event (closing the window)
            if(event.type == SDL_QUIT){
                isRunning = false;
            }else if(event.type == SDL_KEYDOWN){
                // map keyboard inputs to chip-8 keyboard
                switch(event.key.keysym.sym){
                    case SDLK_x: chip8.keyboard[0] = 1; break;
                    case SDLK_1: chip8.keyboard[1] = 1; break;
                    case SDLK_2: chip8.keyboard[2] = 1; break;
                    case SDLK_3: chip8.keyboard[3] = 1; break;
                    case SDLK_q: chip8.keyboard[4] = 1; break;
                    case SDLK_w: chip8.keyboard[5] = 1; break;
                    case SDLK_e: chip8.keyboard[6] = 1; break;
                    case SDLK_a: chip8.keyboard[7] = 1; break;
                    case SDLK_s: chip8.keyboard[8] = 1; break;
                    case SDLK_d: chip8.keyboard[9] = 1; break;
                    case SDLK_z: chip8.keyboard[0xA] = 1; break;
                    case SDLK_c: chip8.keyboard[0xB] = 1; break;
                    case SDLK_4: chip8.keyboard[0xC] = 1; break;
                    case SDLK_r: chip8.keyboard[0xD] = 1; break;
                    case SDLK_f: chip8.keyboard[0xE] = 1; break;
                    case SDLK_v: chip8.keyboard[0xF] = 1; break;
                    case SDLK_LCTRL: chooseROM(chip8); break;
                }
            }else if(event.type == SDL_KEYUP){
                switch(event.key.keysym.sym){
                    case SDLK_x: chip8.keyboard[0] = 0; break;
                    case SDLK_1: chip8.keyboard[1] = 0; break;
                    case SDLK_2: chip8.keyboard[2] = 0; break;
                    case SDLK_3: chip8.keyboard[3] = 0; break;
                    case SDLK_q: chip8.keyboard[4] = 0; break;
                    case SDLK_w: chip8.keyboard[5] = 0; break;
                    case SDLK_e: chip8.keyboard[6] = 0; break;
                    case SDLK_a: chip8.keyboard[7] = 0; break;
                    case SDLK_s: chip8.keyboard[8] = 0; break;
                    case SDLK_d: chip8.keyboard[9] = 0; break;
                    case SDLK_z: chip8.keyboard[0xA] = 0; break;
                    case SDLK_c: chip8.keyboard[0xB] = 0; break;
                    case SDLK_4: chip8.keyboard[0xC] = 0; break;
                    case SDLK_r: chip8.keyboard[0xD] = 0; break;
                    case SDLK_f: chip8.keyboard[0xE] = 0; break;
                    case SDLK_v: chip8.keyboard[0xF] = 0; break;
                }
            }
        }

        // emulate the cpu clock at 60hz
        for(int i = 0; i < 12; i++){
            chip8.cycle();
        }

        // decrement timers at 60Hz
        if(chip8.delay_timer > 0){
            chip8.delay_timer--;
        }
        if(chip8.sound_timer > 0){
            chip8.sound_timer--;
        }

        // transform the chip-8 display into 32-bits colors
        for(int i = 0; i < 64 * 32; i++){
            if(chip8.display[i] == 1){
                pixels[i] = 0xFFFFFFFF; // white
            }else{
                pixels[i] = 0xFF000000; // black
            }
        }

        // update the texture
        SDL_UpdateTexture(texture, nullptr, pixels, 64 * sizeof(uint32_t));
        // clear the renderer
        SDL_RenderClear(renderer);
        // copy the texture to the renderer
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        // update the renderer
        SDL_RenderPresent(renderer);
        // sleep for 16 milliseconds to maintain 60 frames per second
        SDL_Delay(16);

    }

    // close the application
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}