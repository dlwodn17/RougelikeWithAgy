@echo off
setlocal enabledelayedexpansion

echo ===================================================================
echo   RougelikeWithAgy - Elemental Reaction x Weather Forecast Roguelike
echo ===================================================================

set PATH=C:\mingw64\bin;%PATH%
set TMP=C:\Temp
set TEMP=C:\Temp

if not exist "C:\Temp" mkdir "C:\Temp"

echo [1/2] Checking compiler...
where g++ >nul 2>&1
if %errorlevel% neq 0 (
    echo Error: g++ compiler not found in PATH or C:\mingw64\bin.
    pause
    exit /b 1
)

echo [2/2] Compiling and Linking RougelikeWithAgy.exe (Modular Build)...
if not exist libraylib.a (
    echo Building Raylib static archive...
    gcc -O2 -c third_party/raylib/src/rglfw.c third_party/raylib/src/rcore.c third_party/raylib/src/rshapes.c third_party/raylib/src/rtextures.c third_party/raylib/src/rtext.c third_party/raylib/src/rmodels.c third_party/raylib/src/raudio.c third_party/raylib/src/utils.c -Ithird_party/raylib/src -Ithird_party/raylib/src/external/glfw/include -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33
    ar rcs libraylib.a rglfw.o rcore.o rshapes.o rtextures.o rtext.o rmodels.o raudio.o utils.o
    del rglfw.o rcore.o rshapes.o rtextures.o rtext.o rmodels.o raudio.o utils.o
)

g++ -std=c++17 -Wall -Wextra -O2 -Iinclude -Ithird_party/raylib/src src/main.cpp src/GameState.cpp src/Entities/Entity.cpp src/Entities/Player.cpp src/Entities/Enemy.cpp src/Entities/Skill.cpp src/Systems/ElementalSystem.cpp src/Systems/WeatherSystem.cpp src/Systems/CombatSystem.cpp src/Renderer/ParticleSystem.cpp src/Renderer/CombatRenderer.cpp src/Renderer/UIRenderer.cpp -L. -lraylib -lopengl32 -lgdi32 -lwinmm -o RougelikeWithAgy.exe

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Build failed.
    pause
    exit /b %errorlevel%
)

echo.
echo ===================================================================
echo   Build Successful! Launching RougelikeWithAgy.exe...
echo ===================================================================
start "" "RougelikeWithAgy.exe"
