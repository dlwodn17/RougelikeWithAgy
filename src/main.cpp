#include "raylib.h"
#include "GameState.hpp"
#include "Common.hpp"

int main() {
    // Window & Display Configuration for 2560x1440 & Fullscreen support
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI);
    
    // Initial display sizing
    int monitor = GetCurrentMonitor();
    int monWidth = GetMonitorWidth(monitor);
    int monHeight = GetMonitorHeight(monitor);

    // Target 2560x1440 native resolution, or fit monitor initially
    int initWinWidth = 2560;
    int initWinHeight = 1440;
    if (monWidth > 0 && monHeight > 0 && (monWidth < 2560 || monHeight < 1440)) {
        initWinWidth = (int)(monWidth * 0.9f);
        initWinHeight = (int)(monHeight * 0.9f);
    }

    InitWindow(initWinWidth, initWinHeight, "RougelikeWithAgy - 2560x1440 Fullscreen Elemental Roguelike");
    SetWindowMinSize(960, 540);
    SetTargetFPS(60);

    // Create 2560x1440 Native Virtual Canvas
    RenderTexture2D targetCanvas = LoadRenderTexture(ScreenConfig::VIRTUAL_WIDTH, ScreenConfig::VIRTUAL_HEIGHT);
    SetTextureFilter(targetCanvas.texture, TEXTURE_FILTER_BILINEAR);

    // Initialize Game Architecture
    GameManager game;
    game.Initialize();

    // Main Game Loop
    while (!WindowShouldClose()) {
        // Fullscreen Toggle Hotkeys: F11 or Alt+Enter
        if (IsKeyPressed(KEY_F11) || (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_ENTER))) {
            ScreenConfig::ToggleGameFullscreen();
        }

        float dt = GetFrameTime();
        if (dt > 0.1f) dt = 0.1f; // Cap delta time to prevent spiral of death on lag spikes

        // 1. Update Game Logic
        game.Update(dt);

        // 2. Render Scene to 2560x1440 Canvas
        BeginTextureMode(targetCanvas);
        ClearBackground((Color){ 12, 16, 24, 255 });
        game.Draw();
        EndTextureMode();

        // 3. Render Canvas Scaled to Window / Fullscreen Screen Buffer (with letterboxing)
        BeginDrawing();
        ClearBackground(BLACK);

        int currentW = GetScreenWidth();
        int currentH = GetScreenHeight();
        float scale = std::min((float)currentW / (float)ScreenConfig::VIRTUAL_WIDTH, (float)currentH / (float)ScreenConfig::VIRTUAL_HEIGHT);
        
        Rectangle srcRec = { 0.0f, 0.0f, (float)ScreenConfig::VIRTUAL_WIDTH, -(float)ScreenConfig::VIRTUAL_HEIGHT };
        Rectangle dstRec = {
            ((float)currentW - ((float)ScreenConfig::VIRTUAL_WIDTH * scale)) * 0.5f,
            ((float)currentH - ((float)ScreenConfig::VIRTUAL_HEIGHT * scale)) * 0.5f,
            (float)ScreenConfig::VIRTUAL_WIDTH * scale,
            (float)ScreenConfig::VIRTUAL_HEIGHT * scale
        };

        DrawTexturePro(targetCanvas.texture, srcRec, dstRec, (Vector2){ 0, 0 }, 0.0f, WHITE);
        EndDrawing();
    }

    // Cleanup & Exit
    UnloadRenderTexture(targetCanvas);
    CloseWindow();
    return 0;
}
