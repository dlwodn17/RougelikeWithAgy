cmake_minimum_required(VERSION 3.16)
project(RougelikeWithAgy VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Optimization and Warning Flags
if(MSVC)
    add_compile_options(/W4 /EHsc /O2)
else()
    add_compile_options(-Wall -Wextra -O2)
endif()

# Include directories
include_directories(include)

# Source files (Modular & Decoupled Hierarchy)
set(SOURCES
    src/main.cpp
    src/GameState.cpp
    src/Entities/Entity.cpp
    src/Entities/Player.cpp
    src/Entities/Enemy.cpp
    src/Entities/Skill.cpp
    src/Systems/ElementalSystem.cpp
    src/Systems/WeatherSystem.cpp
    src/Systems/CombatSystem.cpp
    src/Systems/RewardSystem.cpp
    src/Systems/MapSystem.cpp
    src/Core/Localization.cpp
    src/Renderer/ParticleSystem.cpp
    src/Renderer/FontManager.cpp
    src/Renderer/WeatherRenderer.cpp
    src/Renderer/CombatRenderer.cpp
    src/Renderer/RewardRenderer.cpp
    src/Renderer/MapRenderer.cpp
    src/Renderer/UIRenderer.cpp
)

# Raylib Dependency Strategy
# 1. Local third_party directory
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/raylib/CMakeLists.txt")
    message(STATUS "Using local Raylib in third_party/raylib")
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
    set(CUSTOMIZE_BUILD OFF CACHE BOOL "" FORCE)
    add_subdirectory(third_party/raylib EXCLUDE_FROM_ALL)
    include_directories(third_party/raylib/src)
else()
    # 2. System installed find_package
    find_package(raylib 5.0 QUIET)

    # 3. Fallback FetchContent
    if(NOT raylib_FOUND)
        include(FetchContent)
        message(STATUS "Fetching Raylib from GitHub...")
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
        set(CUSTOMIZE_BUILD OFF CACHE BOOL "" FORCE)
        
        FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG 5.5
            GIT_SHALLOW TRUE
        )
        FetchContent_MakeAvailable(raylib)
    endif()
endif()

# Executable Target
add_executable(RougelikeWithAgy ${SOURCES})

target_link_libraries(RougelikeWithAgy PRIVATE raylib)

# Platform specific linkages
if(WIN32)
    target_link_libraries(RougelikeWithAgy PRIVATE winmm gdi32 opengl32 user32 shell32)
elseif(APPLE)
    target_link_libraries(RougelikeWithAgy PRIVATE "-framework OpenGL" "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
elseif(UNIX)
    target_link_libraries(RougelikeWithAgy PRIVATE m pthread dl rt X11)
endif()

# Set working directory to project root for debugging
set_target_properties(RougelikeWithAgy PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)
