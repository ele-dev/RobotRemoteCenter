/*
    file: Application.cpp
    written by Elias Geiger
*/

#include "Application.h"

// definitions //

struct JoystickIdDel {
    void operator()(SDL_JoystickID* p) const noexcept { if (p) SDL_free(p); }
};

using JoystickIdPtr = std::unique_ptr<SDL_JoystickID, JoystickIdDel>;

using namespace Core;

Application::Application()
{
    this->m_window = nullptr;
    this->m_renderer = nullptr;
    this->m_gamepad = nullptr;
    this->m_running = false;
}

Application::~Application()
{
    if(this->m_gamepad != nullptr) {
        SDL_CloseGamepad(this->m_gamepad);
        this->m_gamepad = nullptr;
    }

    if(this->m_renderer != nullptr) {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();

        SDL_DestroyRenderer(this->m_renderer);
        this->m_renderer = nullptr;
    }
    
    if(this->m_window != nullptr) {
        SDL_DestroyWindow(this->m_window);
        this->m_window = nullptr;
    }
    
    SDL_Quit();
}

bool Application::Init() 
{
    bool result = this->InitGui();
    if(!result) {
        return false;
    }

    result = this->InitGamepad();
    if(!result) {
        return false;
    }

    // init additional stuff here
    // ...

    return true;
}

void Application::Run()
{
    this->m_running = true;

    while(this->m_running) 
    {
        this->Update();

        this->Render();
    }
}

void Application::Update() 
{
    SDL_Event event;

    // poll for input events and process them
    SDL_PollEvent(&event);
    if(event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
        this->m_running = false;
    }
    else if(event.type == SDL_EVENT_GAMEPAD_AXIS_MOTION) {
        if(event.gaxis.axis == SDL_GAMEPAD_AXIS_LEFTX) {
            std::cout << "Left stick X axis: " << event.gaxis.value << "\n";
            this->m_leftStickState.xAxisValue = event.gaxis.value;
        }
        if(event.gaxis.axis == SDL_GAMEPAD_AXIS_LEFTY) {
            std::cout << "Left stick Y axis: " << event.gaxis.value << "\n";
            this->m_leftStickState.yAxisValue = event.gaxis.value;
        }
    }
}

void Application::Render()
{
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowBgAlpha(0.3f);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                             ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                             ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
    ImGui::Begin("Simple perf monitor", NULL, flags);
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::Text("Left Joystick: X = %d | Y = %d", this->m_leftStickState.xAxisValue, this->m_leftStickState.yAxisValue);

    // render Gui elements code goes here
    // ...

    ImGui::End();   

    ImGui::Render();
    SDL_SetRenderScale(this->m_renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);    
    SDL_SetRenderDrawColorFloat(this->m_renderer, 255.0f, 255.0f, 255.0f, 1.0f);
    SDL_RenderClear(this->m_renderer);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), this->m_renderer);
    SDL_RenderPresent(this->m_renderer);
}


bool Application::InitGui() 
{
    // init SDL3 API
    if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        SDL_Log("SDL3 Error: %s", SDL_GetError());
        return false;
    }

    // then create window and renderer
    SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    if(!SDL_CreateWindowAndRenderer("Robot Control Center Prototype", window_width, window_height, window_flags, &this->m_window, &this->m_renderer)) {
        SDL_Log("SDL3 Error: %s", SDL_GetError());
        return false;
    }

    // enable vysnc for the renderer (either with or without adaptive sync technology)
    // alternative: 1
    if(!SDL_SetRenderVSync(this->m_renderer, SDL_RENDERER_VSYNC_ADAPTIVE)) {    
        SDL_Log("SDL3 Error: %s", SDL_GetError());
        std::cerr << "Failed to enable Vsync!\n";
        return false;
    }

    if(this->m_renderer == nullptr) {
        std::cerr << "renderer is nullptr!\n";
        return false;
    }

    // move the window to center position on the screen
    if(!SDL_SetWindowPosition(this->m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED)) {
        SDL_Log("SDL3 Error: %s", SDL_GetError());
        // return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::StyleColorsDark();

    // obtain scaling factor from primary monitor
    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    if(main_scale == 0.0f) {
        SDL_Log("SDL3 Error: %s", SDL_GetError());
        std::cerr << "invalid monitor scaling factor!\n";
        return false;
    }

    // setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;

    // setup renderer backend 
    ImGui_ImplSDL3_InitForSDLRenderer(this->m_window, this->m_renderer);
    ImGui_ImplSDLRenderer3_Init(this->m_renderer);

    return true;
}


bool Application::InitGamepad() 
{
    // List all detected joysticks
    int numJoysticks = 0;
    JoystickIdPtr joystickIdList(SDL_GetJoysticks(&numJoysticks));

    std::cout << "Found " << numJoysticks << " joystick(s) \n";

    // Open each joystick that can be interpreted as a Gamepad
    for (int i = 0; i < numJoysticks; i++) {
        SDL_JoystickID jid = joystickIdList.get()[i];
        if (SDL_IsGamepad(jid)) {
            this->m_gamepad = SDL_OpenGamepad(jid);
            if (!this->m_gamepad) {
                std::cerr << "Failed to open gamepad " << jid
                          << ": " << SDL_GetError() << "\n";
                continue;
            }

            const char* name = SDL_GetGamepadName(this->m_gamepad);
            SDL_GamepadType type = SDL_GetGamepadType(this->m_gamepad);

            std::cout << "Opened gamepad ID " << jid
                      << " | Name: " << (name ? name : "Unknown")
                      << " | Type: " << type << "\n";

            return true;
        } else {
            std::cout << "Joystick ID " << jid << " is not recognized as a gamepad \n";
        }
    }

    return false;
}
