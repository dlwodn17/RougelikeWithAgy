#include "raylib.h"
#include "GameState.hpp"

int main() {
    // Window & Display Configuration
    const int screenWidth = 1280;
    const int screenHeight = 720;
    
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "RougelikeWithAgy - Elemental Reaction x Weather Forecast Prototype");
    SetTargetFPS(60);

    // Initialize Game Architecture
    GameManager game;
    game.Initialize();

    // Main Game Loop
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (dt > 0.1f) dt = 0.1f; // Cap delta time to prevent spiral of death on lag spikes

        // 1. Update Game Logic
        game.Update(dt);

        // 2. Render Frame
        BeginDrawing();
        ClearBackground((Color){ 12, 16, 24, 255 });
        game.Draw();
        EndDrawing();
    }

    // Cleanup & Exit
    CloseWindow();
    return 0;
}
