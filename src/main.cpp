/*
    file: main.cpp
    written by Elias Geiger
*/

#include "Application.h"

int main(int arg, char **argc)
{
    Core::Application app;

    bool result = app.Init();
    if(!result) {
        std::cout << "Initialization failed! Exit now. \n";
        return EXIT_FAILURE;
    }

    std::cout << "Initialization complete. Start main loop ... \n";

    app.Run();

    std::cout << "Exit now \n";

    return EXIT_SUCCESS;
}