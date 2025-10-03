cmake_minimum_required(VERSION 3.16)

# Setup the project
project(Robot-Remote LANGUAGES CXX)

# Set the C++ language standard settings
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Static linking of c/c++ std libraries to avoid dragging
add_link_options(-static-libstdc++ -static-libgcc)

################################### Static libraries built from source ##############

# ImGui sources
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)
set(IMGUI_BACKENDS ${IMGUI_DIR}/backends)

# Compile ImGui as static library
add_library(imgui STATIC
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_BACKENDS}/imgui_impl_sdl3.cpp
  ${IMGUI_BACKENDS}/imgui_impl_sdlrenderer3.cpp
  # ${IMGUI_BACKENDS}/imgui_impl_vulkan.cpp
)

# Add include path for ImGui headers
target_include_directories(imgui PUBLIC
  ${IMGUI_DIR}
  ${IMGUI_BACKENDS}
)

#######################################################################################

# Add your source files here
set(SOURCES
    src/main.cpp
    src/Application.cpp
    # add source files here ...
)

# Find packages and libraries
find_package(PkgConfig REQUIRED)
# find_package(Vulkan REQUIRED)
find_package(SDL3 QUIET CONFIG)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Add additional include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    # ${Vulkan_INCLUDE_DIRS}
    ${SDL3_INCLUDE_DIRS}
)

# Link the necessary libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    # Vulkan::Vulkan
    SDL3::SDL3
    imgui
)