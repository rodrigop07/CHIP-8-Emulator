#include "chip8.cpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_sdlrenderer2.h"
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <filesystem>
#include <atomic>
#include <SDL2/SDL.h>

namespace fs = std::filesystem;

// audio variables
const int SAMPLE_RATE = 44100;
const int AMPLITUDE = 3000;
const double PI = 3.1415;
double audio_phase = 0.0;
// flag to indicate if the chip-8 is beeping
std::atomic<bool> is_beeping(false);

// audio callback to generate the beep sound
void audioCallback(void* userdata, Uint8* stream, int len){
    int16_t* buffer = (int16_t*)stream;
    int length = len / 2;

    for(int i = 0; i < length; i++){
        if(is_beeping){
            if(std::sin(audio_phase) > 0.0){
                buffer[i] = AMPLITUDE;
            }else{
                buffer[i] = -AMPLITUDE;
            }
            audio_phase += (2.0 * PI * 440.0) / SAMPLE_RATE;
            if(audio_phase > 2.0 * PI){
                audio_phase -= 2.0 * PI;
            }
        }else{
            buffer[i] = 0;
            audio_phase = 0.0;
        }
    }

}


int main(int argc, char* args[]){
    
    // try to init video
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0){
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

    // dear imgui init
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    // dark mode
    ImGui::StyleColorsDark();

    // init sdl renderer for imgui
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);
    
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
    
    // initialize audio
    SDL_AudioSpec want, have;
    std::memset(&want, 0, sizeof(want));
    want.freq = SAMPLE_RATE;
    want.format = AUDIO_S16SYS;
    want.channels = 1;
    want.samples = 2048;
    want.callback = audioCallback;
    
    // open audio device
    SDL_AudioDeviceID audioDevice = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
    if(audioDevice == 0){
        std::cerr << "Failed to open audio:  " << SDL_GetError() << std::endl;
    }else{
        SDL_PauseAudioDevice(audioDevice, 0);
    }
    
    // create buffer to copy the display array
    uint32_t pixels[64 * 32];
    // main event loop
    bool isRunning = true;
    SDL_Event event;
    
    // present the renderer at the start
    SDL_RenderPresent(renderer);

    Chip8 chip8;
    bool rom_loaded = false;
    bool show_rom_menu = false;
    std::vector<std::string> roms_list;
    
    while(isRunning){
        while(SDL_PollEvent(&event) != 0){
            // handle imgui events
            ImGui_ImplSDL2_ProcessEvent(&event);
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
                    //case SDLK_LCTRL: chooseROM(chip8); break;
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

        // emmulate cpu only if a rom is loaded
        if(rom_loaded){
            // emulate the cpu clock at 60hz
            for(int i = 0; i < 15; i++){
                chip8.cycle();
            }

            // decrement timers at 60Hz
            if(chip8.delay_timer > 0){
                chip8.delay_timer--;
                // send audio pulse
                is_beeping = true;
            }else{
                // don't send audio pulse
                is_beeping = false;
            }

        }else{
            is_beeping = false;
        }


        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        if(ImGui::BeginMainMenuBar()){
            if(ImGui::BeginMenu("Archive")){
                if(ImGui::MenuItem("Load ROM")){
                    roms_list.clear();
                    std::string dir_path = "./roms";

                    if(std::filesystem::exists(dir_path)){
                        for(const auto& arc: std::filesystem::directory_iterator(dir_path)){
                            roms_list.push_back(arc.path().string());
                        }
                    }
                    show_rom_menu = true;
                }
                ImGui::Separator();
                if(ImGui::MenuItem("Exit")) isRunning = false;
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if(show_rom_menu){
            ImGui::Begin("Select game", &show_rom_menu);

            if(roms_list.empty()){
                ImGui::Text("No ROMs found in ./roms");
            }else{
                ImGui::BeginChild("ROMsList", ImVec2(0, 150), true);
                for(const std::string& rom_path: roms_list){
                    // extract only the final name of the file
                    std::string file_name = std::filesystem::path(rom_path).filename().string();

                    // if user click on a ROM
                    if(ImGui::Selectable(file_name.c_str())){
                        // reset chip8
                        chip8.reset();

                        if(chip8.loadROM(rom_path)){
                            // unclock cpu
                            rom_loaded = true;
                            // close rom selection menu
                            show_rom_menu = false;
                        }
                    }
                }
                ImGui::EndChild();
            }
            ImGui::End();
        }


        if(chip8.draw_flag || !rom_loaded){
            // transform the chip-8 display into 32-bits colors
            for(int i = 0; i < 64 * 32; i++){
                if(chip8.display[i] == 1){
                    pixels[i] = 0xFFFFFFFF; // white
                }else{
                    pixels[i] = 0xFF000000; // black
                }
            }
            SDL_UpdateTexture(texture, nullptr, pixels, 64 * sizeof(uint32_t));
            // reset draw flag
            chip8.draw_flag = false;
        }

        ImGui::Render();
        // clear the renderer
        SDL_RenderClear(renderer);
        // copy the texture to the renderer
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        // draw imgui frame
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        // update the renderer
        SDL_RenderPresent(renderer);
        // sleep for 16 milliseconds to maintain 60 frames per second
        SDL_Delay(16);
    }

    // close the application
    SDL_CloseAudioDevice(audioDevice);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}