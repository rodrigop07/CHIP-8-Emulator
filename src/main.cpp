#include "chip8.h"
#include "disassembler.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_sdlrenderer2.h"
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstring>
#include <filesystem>
#include <atomic>
#include <SDL2/SDL.h>

namespace fs = std::filesystem;

// window resolutions
const int WIDTH = 1280;
const int HEIGHT = 640;

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
        WIDTH, // width
        HEIGHT, // height
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

    // custom dark style
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding    = 3.0f;
    style.FrameRounding     = 2.0f;
    style.PopupRounding     = 2.0f;
    style.ScrollbarRounding = 2.0f;
    style.GrabRounding      = 2.0f;
    style.WindowPadding     = ImVec2(16.0f, 14.0f);
    style.FramePadding      = ImVec2(10.0f, 6.0f);
    style.ItemSpacing       = ImVec2(10.0f, 8.0f);
    style.ScrollbarSize     = 12.0f;

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg]         = ImVec4(0.06f, 0.08f, 0.13f, 0.95f);
    colors[ImGuiCol_ChildBg]          = ImVec4(0.04f, 0.06f, 0.10f, 1.00f);
    colors[ImGuiCol_Border]           = ImVec4(0.18f, 0.28f, 0.45f, 0.70f);
    colors[ImGuiCol_FrameBg]          = ImVec4(0.08f, 0.12f, 0.22f, 1.00f);
    colors[ImGuiCol_FrameBgHovered]   = ImVec4(0.12f, 0.18f, 0.32f, 1.00f);
    colors[ImGuiCol_FrameBgActive]    = ImVec4(0.15f, 0.22f, 0.40f, 1.00f);
    colors[ImGuiCol_TitleBg]          = ImVec4(0.05f, 0.08f, 0.15f, 1.00f);
    colors[ImGuiCol_TitleBgActive]    = ImVec4(0.08f, 0.14f, 0.28f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]      = ImVec4(0.04f, 0.06f, 0.10f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab]    = ImVec4(0.18f, 0.35f, 0.60f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.25f, 0.48f, 0.80f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.30f, 0.58f, 1.00f, 1.00f);
    colors[ImGuiCol_Button]           = ImVec4(0.12f, 0.22f, 0.45f, 1.00f);
    colors[ImGuiCol_ButtonHovered]    = ImVec4(0.18f, 0.35f, 0.68f, 1.00f);
    colors[ImGuiCol_ButtonActive]     = ImVec4(0.22f, 0.45f, 0.85f, 1.00f);
    colors[ImGuiCol_Header]           = ImVec4(0.14f, 0.25f, 0.50f, 0.80f);
    colors[ImGuiCol_HeaderHovered]    = ImVec4(0.20f, 0.36f, 0.65f, 0.90f);
    colors[ImGuiCol_HeaderActive]     = ImVec4(0.25f, 0.45f, 0.80f, 1.00f);
    colors[ImGuiCol_Separator]        = ImVec4(0.15f, 0.25f, 0.45f, 0.80f);
    colors[ImGuiCol_CheckMark]        = ImVec4(0.35f, 0.65f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab]       = ImVec4(0.25f, 0.48f, 0.85f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.35f, 0.60f, 1.00f, 1.00f);
    colors[ImGuiCol_Text]             = ImVec4(0.85f, 0.90f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]     = ImVec4(0.35f, 0.42f, 0.55f, 1.00f);

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

    // cpu
    Chip8 chip8;
    // disassembler
    Disassembler disassembler;
    //
    std::vector<std::string> instructions_list;
    std::string current_rom_path;
    bool rom_loaded = false;
    bool show_main_menu = false;
    bool show_rom_menu = false;
    bool show_debug = false;
    bool is_paused = false;
    int inst_per_frame = 15;
    std::vector<std::string> roms_list;
    char rom_search_buf[128] = {};
    int selected_rom_index = -1;

    const int keypad_layout[16] = {
        0x1, 0x2, 0x3, 0xC,
        0x4, 0x5, 0x6, 0xD,
        0x7, 0x8, 0x9, 0xE,
        0xA, 0x0, 0xB, 0xF
    };

    const char* key_map[16] = {
        "1", "2", "3", "C",
        "4", "5", "6", "D",
        "7", "8", "9", "E",
        "A", "0", "B", "F"
    };
    
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
                    case SDLK_ESCAPE:
                        show_main_menu = !show_main_menu;
                        if(!show_main_menu) show_rom_menu = false;
                        break;
                    case SDLK_TAB:
                        show_debug = !show_debug;
                        break;
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
        if(rom_loaded && !show_main_menu && !is_paused){
            // emulate the cpu clock at 60hz
            for(int i = 0; i < inst_per_frame; i++){
                chip8.cycle();
            }

            // decrement timers at 60Hz
            if(chip8.delay_timer > 0){
                chip8.delay_timer--;
            }

            if(chip8.sound_timer > 0){
                chip8.sound_timer--;
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
        
        if(show_main_menu || !rom_loaded){
            // center the menu window
            ImGui::SetNextWindowPos(ImVec2(WIDTH / 2.0f, HEIGHT / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(260, 0), ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.96f);

            ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
                                     ImGuiWindowFlags_NoSavedSettings |
                                     ImGuiWindowFlags_NoFocusOnAppearing |
                                     ImGuiWindowFlags_NoNav |
                                     ImGuiWindowFlags_NoMove |
                                     ImGuiWindowFlags_AlwaysAutoResize;

            if(ImGui::Begin("##MainMenu", nullptr, flags)){
                // title
                float title_w = ImGui::CalcTextSize("CHIP-8 EMULATOR").x;
                ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - title_w) * 0.5f + ImGui::GetStyle().WindowPadding.x);
                ImGui::TextColored(ImVec4(0.40f, 0.70f, 1.00f, 1.0f), "CHIP-8 EMULATOR");

                float sub_w = ImGui::CalcTextSize("v1.0").x;
                ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - sub_w) * 0.5f + ImGui::GetStyle().WindowPadding.x);
                ImGui::TextColored(ImVec4(0.35f, 0.52f, 0.75f, 1.0f), "v1.0");

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                // show current rom name if loaded
                if(rom_loaded && !current_rom_path.empty()){
                    std::string loaded_name = fs::path(current_rom_path).filename().string();
                    ImGui::TextColored(ImVec4(0.50f, 0.85f, 0.60f, 1.0f), "  Loaded: %s", loaded_name.c_str());
                    ImGui::Spacing();
                }

                float btn_w = ImGui::GetContentRegionAvail().x;

                // Load ROM button (blue accent)
                ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.10f, 0.22f, 0.48f, 1.00f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.16f, 0.34f, 0.70f, 1.00f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.20f, 0.44f, 0.90f, 1.00f));
                if(ImGui::Button("  Load ROM", ImVec2(btn_w, 36))){
                    roms_list.clear();
                    selected_rom_index = -1;
                    std::memset(rom_search_buf, 0, sizeof(rom_search_buf));
                    std::string dir_path = "./roms";
                    if(fs::exists(dir_path)){
                        for(const auto& arc: fs::directory_iterator(dir_path)){
                            roms_list.push_back(arc.path().string());
                        }
                        std::sort(roms_list.begin(), roms_list.end());
                    }
                    show_rom_menu = true;
                }
                ImGui::PopStyleColor(3);

                ImGui::Spacing();

                // Resume button (only when a rom is loaded)
                if(rom_loaded){
                    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.12f, 0.35f, 0.18f, 1.00f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.18f, 0.52f, 0.26f, 1.00f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.22f, 0.65f, 0.32f, 1.00f));
                    if(ImGui::Button("  Resume", ImVec2(btn_w, 36))){
                        show_main_menu = false;
                        show_rom_menu  = false;
                    }
                    ImGui::PopStyleColor(3);
                    ImGui::Spacing();
                }

                // Exit button (danger color)
                ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.45f, 0.10f, 0.10f, 1.00f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.65f, 0.15f, 0.15f, 1.00f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.80f, 0.20f, 0.20f, 1.00f));
                if(ImGui::Button("  Exit", ImVec2(btn_w, 36))){
                    isRunning = false;
                }
                ImGui::PopStyleColor(3);

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::TextColored(ImVec4(0.28f, 0.38f, 0.55f, 1.0f), "  ESC: toggle menu   TAB: debug");
            }
            ImGui::End();
        }

        if(show_rom_menu){
            ImGui::SetNextWindowPos(ImVec2(WIDTH / 2.0f, HEIGHT / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(420, 340), ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.97f);

            ImGuiWindowFlags rom_flags = ImGuiWindowFlags_NoResize |
                                         ImGuiWindowFlags_NoMove |
                                         ImGuiWindowFlags_NoSavedSettings;

            if(ImGui::Begin("  Select ROM", &show_rom_menu, rom_flags)){
                // search bar
                ImGui::SetNextItemWidth(-1);
                ImGui::InputTextWithHint("##search", "Search...", rom_search_buf, sizeof(rom_search_buf));
                ImGui::Spacing();

                if(roms_list.empty()){
                    ImGui::Spacing();
                    float msg_w = ImGui::CalcTextSize("No ROMs found in ./roms/").x;
                    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - msg_w) * 0.5f + ImGui::GetStyle().WindowPadding.x);
                    ImGui::TextColored(ImVec4(0.70f, 0.40f, 0.40f, 1.0f), "No ROMs found in ./roms/");
                }else{
                    // build filtered list
                    std::string filter(rom_search_buf);
                    std::vector<int> filtered;
                    for(int i = 0; i < (int)roms_list.size(); i++){
                        std::string name = fs::path(roms_list[i]).filename().string();
                        // case-insensitive check
                        std::string name_lw = name, filter_lw = filter;
                        for(auto& c : name_lw)   c = (char)std::tolower((unsigned char)c);
                        for(auto& c : filter_lw) c = (char)std::tolower((unsigned char)c);
                        if(filter.empty() || name_lw.find(filter_lw) != std::string::npos){
                            filtered.push_back(i);
                        }
                    }

                    // count label
                    ImGui::TextColored(ImVec4(0.35f, 0.55f, 0.85f, 1.0f),
                        "%d ROM(s)", (int)filtered.size());
                    ImGui::Spacing();

                    // rom list child window
                    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.04f, 0.06f, 0.12f, 1.0f));
                    ImGui::BeginChild("ROMsList", ImVec2(-1, 200), true);

                    for(int fi = 0; fi < (int)filtered.size(); fi++){
                        int idx = filtered[fi];
                        const std::string& rom_path = roms_list[idx];
                        std::string file_name = fs::path(rom_path).filename().string();

                        bool is_selected = (selected_rom_index == idx);

                        // alternating row background
                        if(fi % 2 == 0){
                            ImVec2 row_min = ImGui::GetCursorScreenPos();
                            ImVec2 row_max = ImVec2(row_min.x + ImGui::GetContentRegionAvail().x,
                                                    row_min.y + ImGui::GetTextLineHeightWithSpacing());
                            ImGui::GetWindowDrawList()->AddRectFilled(
                                row_min, row_max, IM_COL32(20, 18, 35, 120));
                        }

                        if(is_selected){
                            ImGui::PushStyleColor(ImGuiCol_Header,        ImVec4(0.35f, 0.28f, 0.70f, 1.0f));
                            ImGui::PushStyleColor(ImGuiCol_HeaderHovered,  ImVec4(0.42f, 0.34f, 0.80f, 1.0f));
                        }

                        if(ImGui::Selectable(("  " + file_name).c_str(), is_selected,
                                             ImGuiSelectableFlags_AllowDoubleClick)){
                            selected_rom_index = idx;

                            if(ImGui::IsMouseDoubleClicked(0)){
                                chip8.reset();
                                if(chip8.loadROM(rom_path)){
                                    rom_loaded       = true;
                                    show_rom_menu    = false;
                                    show_main_menu   = false;
                                    current_rom_path = rom_path;
                                    instructions_list = disassembler.disassemble(chip8.memory, chip8.rom_size);
                                }
                            }
                        }

                        if(is_selected){
                            ImGui::PopStyleColor(2);
                        }
                    }

                    ImGui::EndChild();
                    ImGui::PopStyleColor();

                    ImGui::Spacing();
                    ImGui::TextColored(ImVec4(0.28f, 0.42f, 0.62f, 1.0f), "Double-click to load");
                    ImGui::SameLine();

                    float load_btn_w = 100.0f;
                    ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x - load_btn_w + ImGui::GetStyle().WindowPadding.x);

                    bool can_load = (selected_rom_index >= 0 && selected_rom_index < (int)roms_list.size());
                    if(!can_load) ImGui::BeginDisabled();

                    ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.10f, 0.22f, 0.48f, 1.00f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.16f, 0.34f, 0.70f, 1.00f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  ImVec4(0.20f, 0.44f, 0.90f, 1.00f));
                    if(ImGui::Button("Load", ImVec2(load_btn_w, 0))){
                        const std::string& rom_path = roms_list[selected_rom_index];
                        chip8.reset();
                        if(chip8.loadROM(rom_path)){
                            rom_loaded       = true;
                            show_rom_menu    = false;
                            show_main_menu   = false;
                            current_rom_path = rom_path;
                            instructions_list = disassembler.disassemble(chip8.memory, chip8.rom_size);
                        }
                    }
                    ImGui::PopStyleColor(3);

                    if(!can_load) ImGui::EndDisabled();
                }
            }
            ImGui::End();
        }

        // debug panel
        if(show_debug){
            // get window width and height
            int win_width, win_height;
            SDL_GetWindowSize(window, &win_width, &win_height);

            // set the ImGui window to full screen
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(win_width, win_height));
            ImGui::SetNextWindowBgAlpha(1.0f);

            ImGuiWindowFlags panel_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus;
            ImGui::Begin("Debug Panel", nullptr, panel_flags);

            ImGui::BeginChild("Instructions", ImVec2(350, 0), true);
            ImGui::Text("Disassembler");

            ImGui::BeginChild("InstructionsList", ImVec2(0, 350), false);


            // logic to keep pc always visible in instructions list
            static uint16_t last_pc = 0;
            if(!ImGui::IsMouseDown(ImGuiMouseButton_Left) && abs((int)chip8.pc - (int)last_pc) > 2){
                float line_height = ImGui::GetTextLineHeightWithSpacing();
                float target_row = (chip8.pc - 0x200) / 2.0f;
                ImGui::SetScrollY(target_row * line_height - (350 / 2.0f));
            }
            last_pc = chip8.pc;

            // create a clipper with the size of instructions_list
            ImGuiListClipper clipper;
            clipper.Begin(instructions_list.size());

            while(clipper.Step()){
                for(int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++){
                    //get instruction
                    std::string instruction = instructions_list[row]; 

                    // check if the current address is where the program counter is pointing to
                    bool is_current_pc = (0x200 + (row * 2) == chip8.pc);

                    // print instruction with pc highlighted
                    if(is_current_pc){
                        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "-> 0x%04X: %s", 0x200 + (row * 2), instruction.c_str());
                    }else{
                        ImGui::Text("0x%04X: %s", 0x200 + (row * 2), instruction.c_str());
                    }
                }
            }
            
            ImGui::EndChild();
            ImGui::Separator();

            ImGui::BeginChild("HardwareControls", ImVec2(0, 0));
            ImGui::Text("Hardware Controls");
            ImGui::Separator();
            ImGui::Checkbox("Pause", &is_paused);
            ImGui::BeginDisabled(!is_paused);
            if(ImGui::Button("Step One")){
                chip8.cycle();
            }
            ImGui::EndDisabled();

            ImGui::SetNextItemWidth(120);
            if(ImGui::InputInt("Cycles/Frame", &inst_per_frame)){
                if(inst_per_frame < 1) inst_per_frame = 1;
                if(inst_per_frame > 1000) inst_per_frame = 1000;
            }

            if(ImGui::Button("Reset ROM")){
                chip8.reset();
                chip8.loadROM(current_rom_path);
                instructions_list = disassembler.disassemble(chip8.memory, chip8.rom_size);
            }

            ImGui::SameLine();

            if(ImGui::Button("Load ROM")){
                show_rom_menu = true;
            }

            for(int i = 0; i < 16; i++){
                if(i % 4 != 0) ImGui::SameLine();

                int index = keypad_layout[i];
                
                if(chip8.keyboard[index]){
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
                }
                
                ImGui::Button(key_map[i], ImVec2(25, 25));
                
                if(chip8.keyboard[index]){
                    ImGui::PopStyleColor();
                }
            }


            ImGui::EndChild();

            ImGui::EndChild();
            ImGui::SameLine();

            ImGui::BeginChild("MidColumn", ImVec2(540, 0), false);

            // define the area for the game
            float game_area_width = 530.0f;
            float game_are_height = 284.0f;
            ImGui::BeginChild("GameScreen", ImVec2(0, game_are_height), true);
            //ImGui::Text("Emulator Display");
            
            // calculate display size using available area to the game
            float ch8_width = game_area_width - 12.0f;
            float ch8_height = ch8_width / 2.0f;
            
            // send the SDL texture to ImGui as a image
            ImGui::Image((ImTextureID)(intptr_t)texture, ImVec2(ch8_width, ch8_height));
            ImGui::EndChild();
            
            //ImGui::BeginChild("Memory", ImVec2(256, 256), true);
            // create collapsing header for memory
            //if(ImGui::CollapsingHeader("Memory")){
                ImGui::BeginChild("MemoryView", ImVec2(0, 200), true);
                ImGui::Text("Memory");
                // create list clipper to display memory
                //ImGuiListClipper clipper;
                // 16 bytes per line, 256 lines in total
                clipper.Begin(256);

                // the clipper will step for each line that is visible in the window
                while(clipper.Step()){
                    for(int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++){
                        // get the base address for the current line
                        int baseAddr = row * 16;
                        // display address
                        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "0x%04X: ", baseAddr);

                        // draw the 16 bytes of the current line
                        for(int col = 0; col < 16; col++){
                            ImGui::SameLine();
                            
                            if(baseAddr + col == chip8.pc || baseAddr + col == chip8.pc + 1){
                                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%02X", chip8.memory[baseAddr + col]);
                                
                            }else{
                                ImGui::Text("%02X", chip8.memory[baseAddr + col]);
                            }
                        }
                        
                    }
                    
                }
                ImGui::EndChild();
            //}
            
            //ImGui::EndChild();
            ImGui::EndChild();

            ImGui::SameLine();

            // hardware state area
            ImGui::BeginChild("HardwareScreen", ImVec2(350, 0), true);

            // special registers
            ImGui::Text("Special registers");
            ImGui::Text("PC: 0x%04X", chip8.pc);
            ImGui::Text("Index Register: 0x%04X", chip8.I);
            ImGui::Separator();
            
            // time registers
            ImGui::Text("Timers");
            ImGui::Text("Delay Timer: 0x%02X", chip8.delay_timer);
            ImGui::Text("Sound Timer: 0x%02X", chip8.sound_timer);
            ImGui::Separator();

            // general purpose registers
            ImGui::Text("V Registers");
            for(int i = 0; i < 16; i++){
                ImGui::Text("V%X - 0x%02X", i, chip8.V[i]);

                if((i + 1) % 4 != 0){
                    ImGui::SameLine();
                }
            }
            ImGui::Separator();

            // stack and sp
            ImGui::Text("Stack Pointer: 0x%04X", chip8.sp);
            ImGui::Text("Stack:");
            for(int i = 0; i < 16; i++){
                if(i == chip8.sp){
                    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "0x%04X     (%d) <-", chip8.stack[i], i);
                }else{
                    ImGui::Text("0x%04X     (%d)", chip8.stack[i], i);
                }
            }
            ImGui::Separator();


            ImGui::EndChild();
            // close the debug window
            ImGui::End();
        }


        
        if(chip8.draw_flag){
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
        if(!show_debug){
            // copy the texture to the renderer
            SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        }
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