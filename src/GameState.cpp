#include "GameState.hpp"

GameManager::GameManager() 
    : state(AppState::TITLE), showSettings(false), showGuide(false), selectedSettingIdx(0) {
}

void GameManager::Initialize() {
    renderer.init();
    combatSystem.InitializeNewRun();
}

void GameManager::Update(float dt) {
    // 1. Settings Overlay Navigation
    if (showSettings) {
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_O)) {
            showSettings = false;
            return;
        }

        // Navigate options row
        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
            selectedSettingIdx = (selectedSettingIdx - 1 + 3) % 3;
        }
        if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
            selectedSettingIdx = (selectedSettingIdx + 1) % 3;
        }

        // Change values in active row
        if (selectedSettingIdx == 0) { // Resolution Row
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
                DisplaySettings::CycleResolution(-1);
            }
            if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
                DisplaySettings::CycleResolution(1);
            }
            if (IsKeyPressed(KEY_ONE)) DisplaySettings::SetResolutionIndex(0);
            if (IsKeyPressed(KEY_TWO)) DisplaySettings::SetResolutionIndex(1);
            if (IsKeyPressed(KEY_THREE)) DisplaySettings::SetResolutionIndex(2);
            if (IsKeyPressed(KEY_FOUR)) DisplaySettings::SetResolutionIndex(3);
        } else if (selectedSettingIdx == 1) { // Display Mode Row
            if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                DisplaySettings::ToggleFullscreenMode();
            }
        } else if (selectedSettingIdx == 2) { // Close Button
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                showSettings = false;
                return;
            }
        }

        // Mouse click triggers
        Vector2 mousePos = ScreenConfig::GetVirtualMousePosition();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            // Resolution buttons
            float y = (float)renderer.getVirtualHeight() * 0.5f - 500.0f + 190.0f;
            float modalX = (float)renderer.getVirtualWidth() * 0.5f - 750.0f;
            for (int i = 0; i < 4; ++i) {
                Rectangle btnRec = { modalX + 60.0f + (float)i * 340.0f, y, 320.0f, 100.0f };
                if (CheckCollisionPointRec(mousePos, btnRec)) {
                    DisplaySettings::SetResolutionIndex(i);
                    selectedSettingIdx = 0;
                }
            }

            // Display mode buttons
            float yMode = y + 185.0f;
            Rectangle winBtnRec = { modalX + 60.0f, yMode, 400.0f, 90.0f };
            Rectangle fsBtnRec = { modalX + 490.0f, yMode, 400.0f, 90.0f };
            if (CheckCollisionPointRec(mousePos, winBtnRec)) {
                if (IsWindowFullscreen()) DisplaySettings::ToggleFullscreenMode();
                selectedSettingIdx = 1;
            }
            if (CheckCollisionPointRec(mousePos, fsBtnRec)) {
                if (!IsWindowFullscreen()) DisplaySettings::ToggleFullscreenMode();
                selectedSettingIdx = 1;
            }

            // Close button
            Rectangle closeRec = { modalX + 1500.0f * 0.5f - 180.0f, (float)renderer.getVirtualHeight() * 0.5f - 500.0f + 1000.0f - 90.0f, 360.0f, 65.0f };
            if (CheckCollisionPointRec(mousePos, closeRec)) {
                showSettings = false;
                return;
            }
        }
        return;
    }

    // 2. Guide Overlay Dismissal
    if (showGuide) {
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_H) || IsKeyPressed(KEY_TAB)) {
            showGuide = false;
            return;
        }
        Vector2 mousePos = ScreenConfig::GetVirtualMousePosition();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            float modalX = (float)renderer.getVirtualWidth() * 0.5f - 700.0f;
            float modalY = (float)renderer.getVirtualHeight() * 0.5f - 480.0f;
            Rectangle closeRec = { modalX + 1400.0f * 0.5f - 140.0f, modalY + 960.0f - 90.0f, 280.0f, 60.0f };
            if (CheckCollisionPointRec(mousePos, closeRec)) {
                showGuide = false;
                return;
            }
        }
        return;
    }

    // Hotkeys to open menus
    if (IsKeyPressed(KEY_O)) {
        showSettings = true;
        showGuide = false;
        return;
    }
    if (IsKeyPressed(KEY_H) || IsKeyPressed(KEY_TAB)) {
        showGuide = true;
        showSettings = false;
        return;
    }

    // Check Header Button Clicks during BATTLE or TITLE
    Vector2 mousePos = ScreenConfig::GetVirtualMousePosition();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (state == AppState::BATTLE) {
            Rectangle helpRec = { (float)renderer.getVirtualWidth() - 680.0f, 35.0f, 200.0f, 80.0f };
            Rectangle optRec = { (float)renderer.getVirtualWidth() - 460.0f, 35.0f, 210.0f, 80.0f };
            Rectangle fsRec = { (float)renderer.getVirtualWidth() - 230.0f, 35.0f, 190.0f, 80.0f };

            if (CheckCollisionPointRec(mousePos, helpRec)) {
                showGuide = true;
                return;
            }
            if (CheckCollisionPointRec(mousePos, optRec)) {
                showSettings = true;
                return;
            }
            if (CheckCollisionPointRec(mousePos, fsRec)) {
                DisplaySettings::ToggleFullscreenMode();
                return;
            }
        } else if (state == AppState::TITLE) {
            float titleX = (float)renderer.getVirtualWidth() * 0.5f - 650.0f;
            Rectangle optRec = { titleX + 1300.0f * 0.5f - 260.0f, 160.0f + 640.0f, 520.0f, 75.0f };
            Rectangle fsRec = { titleX + 1300.0f * 0.5f - 260.0f, 160.0f + 730.0f, 520.0f, 75.0f };

            if (CheckCollisionPointRec(mousePos, optRec)) {
                showSettings = true;
                return;
            }
            if (CheckCollisionPointRec(mousePos, fsRec)) {
                DisplaySettings::ToggleFullscreenMode();
                return;
            }
        }
    }

    // Main State Progression
    switch (state) {
        case AppState::TITLE:
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                combatSystem.InitializeNewRun();
                state = AppState::BATTLE;
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                float titleX = (float)renderer.getVirtualWidth() * 0.5f - 650.0f;
                Rectangle startRec = { titleX + 1300.0f * 0.5f - 260.0f, 160.0f + 540.0f, 520.0f, 85.0f };
                if (CheckCollisionPointRec(mousePos, startRec)) {
                    combatSystem.InitializeNewRun();
                    state = AppState::BATTLE;
                }
            }
            break;

        case AppState::BATTLE:
            combatSystem.Update(dt);

            // Check if CombatSystem entered victory or defeat
            if (combatSystem.GetPhase() == CombatPhase::VICTORY_SCREEN) {
                state = AppState::VICTORY;
            } else if (combatSystem.GetPhase() == CombatPhase::DEFEAT_SCREEN) {
                state = AppState::GAME_OVER;
            }

            // Quick restart hotkey
            if (IsKeyPressed(KEY_R)) {
                combatSystem.RestartGame();
            }
            break;

        case AppState::VICTORY:
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                if (combatSystem.GetCurrentWave() >= combatSystem.GetMaxWaves()) {
                    combatSystem.RestartGame();
                } else {
                    combatSystem.NextWave();
                }
                state = AppState::BATTLE;
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Rectangle nextRec = { (float)renderer.getVirtualWidth() * 0.5f - 220.0f, (float)renderer.getVirtualHeight() * 0.5f - 350.0f + 480.0f, 440.0f, 90.0f };
                if (CheckCollisionPointRec(mousePos, nextRec)) {
                    if (combatSystem.GetCurrentWave() >= combatSystem.GetMaxWaves()) {
                        combatSystem.RestartGame();
                    } else {
                        combatSystem.NextWave();
                    }
                    state = AppState::BATTLE;
                }
            }
            break;

        case AppState::GAME_OVER:
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                combatSystem.RestartGame();
                state = AppState::BATTLE;
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Rectangle retryRec = { (float)renderer.getVirtualWidth() * 0.5f - 220.0f, (float)renderer.getVirtualHeight() * 0.5f - 350.0f + 480.0f, 440.0f, 90.0f };
                if (CheckCollisionPointRec(mousePos, retryRec)) {
                    combatSystem.RestartGame();
                    state = AppState::BATTLE;
                }
            }
            break;

        default:
            break;
    }
}

void GameManager::Draw() {
    renderer.render(combatSystem, combatSystem.GetParticleSystem(), state, selectedSettingIdx, showGuide, showSettings);
}
