#include "chip8.cpp"
#include <iostream>
#include <SDL2/SDL.h>

int main(int argc, char* args[]){
    // initialize chip-8 emulator
    Chip8 chip8;
    if(!chip8.loadROM("Airplane.ch8")){
        std::cerr << "Failed to load ROM" << std::endl;
        return -1;
    }

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
        640, // width
        320, // height
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
            if(event.type == SDL_QUIT){
                isRunning = false;
            }
        }

        // emulate the cpu cycle
        chip8.cycle();
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