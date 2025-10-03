cmake_minimum_required(VERSION 3.16)

# Setup the project
project(Robot-Remote LANGUAGES CXX)

# Set the C++ language standard settings
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Static linking of c/c++ std libraries to avoid dragging
add_link_options(-static-libstdc++ -static-libgcc)

# ImGui directories
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)
set(IMGUI_BACKENDS ${IMGUI_DIR}/backends)

# Add your source files here
set(SOURCES
    # Imgui sources
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_BACKENDS}/imgui_impl_sdl3.cpp
    ${IMGUI_BACKENDS}/imgui_impl_sdlrenderer3.cpp

    src/main.cpp
    src/Application.cpp
    # add source files here ...
)

# Find packages and libraries
find_package(PkgConfig REQUIRED)
find_package(SDL3 QUIET CONFIG)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Add additional include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_BACKENDS}
    ${SDL3_INCLUDE_DIRS}
)

# Link the necessary libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3
)