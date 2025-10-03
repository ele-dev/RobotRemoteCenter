/*
    file: Application.h
    written by Elias Geiger
*/

#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_joystick.h>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include <iostream>
#include <vector>
#include <memory>

namespace Core
{
    const int window_width = 1280;
    const int window_height = 720;
    const std::string app_name = "Robot Remote Center";
    const std::string app_version = "1.0.0";
    const std::string app_identifier = "com.hs-esslingen.apps.robotremotecenter";

    struct AnalogJoystickState
    {
        int xAxisValue = 0;
        int yAxisValue = 0;
    };

    class Application
    {
    public:
        Application();
        virtual ~Application();

        bool Init();
        void Run();

    private:
        bool InitGui();
        bool InitGamepad();

        void Update();
        void Render();

    private:
        SDL_Window* m_window;
        SDL_Renderer* m_renderer;
        SDL_Gamepad* m_gamepad;

        bool m_running;
        AnalogJoystickState m_leftStickState = {};
    };
}
