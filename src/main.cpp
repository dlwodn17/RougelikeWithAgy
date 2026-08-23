#include "raylib.h"
#include "GameState.hpp"
#include "Common.hpp"

int main() {
    // Window & Display Configuration (No FLAG_WINDOW_HIGHDPI to prevent Windows viewport scaling mismatches)
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    
    // Auto-detect best initial window size based on monitor resolution
    int monitor = GetCurrentMonitor();
    int monWidth = GetMonitorWidth(monitor);
    int monHeight = GetMonitorHeight(monitor);

    DisplaySettings::AutoDetectDefaultResolution();
    int defaultIdx = DisplaySettings::GetCurrentResolutionIndex();
    const auto& resList = DisplaySettings::GetResolutions();
    int initWinWidth = resList[defaultIdx].width;
    int initWinHeight = resList[defaultIdx].height;

    // Safety clamp for windowed borders
    if (monWidth > 0 && monHeight > 0) {
        if (initWinWidth >= monWidth && defaultIdx > 0) {
            defaultIdx--;
            initWinWidth = resList[defaultIdx].width;
            initWinHeight = resList[defaultIdx].height;
            DisplaySettings::SetResolutionIndex(defaultIdx);
        }
    }

    InitWindow(initWinWidth, initWinHeight, "RougelikeWithAgy - Elemental Reaction Roguelike");
    SetWindowMinSize(960, 540);
    SetTargetFPS(60);

    // Center window on primary monitor
    if (monWidth > 0 && monHeight > 0) {
        SetWindowPosition((monWidth - initWinWidth) / 2, (monHeight - initWinHeight) / 2);
    }

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

        // 3. Render Canvas Scaled to Physical Framebuffer (with exact 16:9 aspect ratio)
        BeginDrawing();
        ClearBackground(BLACK);

        int renderW = GetRenderWidth();
        int renderH = GetRenderHeight();
        if (renderW <= 0) renderW = GetScreenWidth();
        if (renderH <= 0) renderH = GetScreenHeight();

        float scale = std::min((float)renderW / (float)ScreenConfig::VIRTUAL_WIDTH,
                               (float)renderH / (float)ScreenConfig::VIRTUAL_HEIGHT);
        
        Rectangle srcRec = { 0.0f, 0.0f, (float)ScreenConfig::VIRTUAL_WIDTH, -(float)ScreenConfig::VIRTUAL_HEIGHT };
        Rectangle dstRec = {
            ((float)renderW - ((float)ScreenConfig::VIRTUAL_WIDTH * scale)) * 0.5f,
            ((float)renderH - ((float)ScreenConfig::VIRTUAL_HEIGHT * scale)) * 0.5f,
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
