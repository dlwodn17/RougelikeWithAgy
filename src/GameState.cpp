#include "GameState.hpp"

GameManager::GameManager() 
    : state(AppState::TITLE), showSettings(false), showGuide(false), selectedSettingIdx(0) {
}

void GameManager::Initialize() {
    uiRenderer.Initialize();
    combatSystem.SetParticleSystem(&particleSystem);
    combatSystem.InitializeNewRun();
}

void GameManager::Update(float dt) {
    Vector2 mousePos = ScreenConfig::GetVirtualMousePosition();

    // 1. Settings Overlay Input Handling
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

        // Mouse click triggers inside Settings Overlay
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            float modalX = (float)uiRenderer.GetVirtualWidth() * 0.5f - 750.0f;
            float modalY = (float)uiRenderer.GetVirtualHeight() * 0.5f - 500.0f;

            // Resolution buttons (row 0)
            float yRes = modalY + 190.0f;
            for (int i = 0; i < 4; ++i) {
                Rectangle btnRec = { modalX + 60.0f + (float)i * 340.0f, yRes, 320.0f, 100.0f };
                if (CheckCollisionPointRec(mousePos, btnRec)) {
                    DisplaySettings::SetResolutionIndex(i);
                    selectedSettingIdx = 0;
                    return;
                }
            }

            // Display mode buttons (row 1)
            float yMode = modalY + 375.0f;
            Rectangle winBtnRec = { modalX + 60.0f, yMode, 400.0f, 90.0f };
            Rectangle fsBtnRec = { modalX + 490.0f, yMode, 400.0f, 90.0f };
            if (CheckCollisionPointRec(mousePos, winBtnRec)) {
                if (IsWindowFullscreen()) DisplaySettings::ToggleFullscreenMode();
                selectedSettingIdx = 1;
                return;
            }
            if (CheckCollisionPointRec(mousePos, fsBtnRec)) {
                if (!IsWindowFullscreen()) DisplaySettings::ToggleFullscreenMode();
                selectedSettingIdx = 1;
                return;
            }

            // Close button (row 2)
            Rectangle closeRec = { modalX + 1500.0f * 0.5f - 180.0f, modalY + 1000.0f - 90.0f, 360.0f, 65.0f };
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
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            float modalX = (float)uiRenderer.GetVirtualWidth() * 0.5f - 700.0f;
            float modalY = (float)uiRenderer.GetVirtualHeight() * 0.5f - 480.0f;
            Rectangle closeRec = { modalX + 1400.0f * 0.5f - 140.0f, modalY + 960.0f - 90.0f, 280.0f, 60.0f };
            if (CheckCollisionPointRec(mousePos, closeRec)) {
                showGuide = false;
                return;
            }
        }
        return;
    }

    // Global Hotkeys to open menus
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
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (state == AppState::BATTLE) {
            Rectangle helpRec = { (float)uiRenderer.GetVirtualWidth() - 680.0f, 35.0f, 200.0f, 80.0f };
            Rectangle optRec = { (float)uiRenderer.GetVirtualWidth() - 460.0f, 35.0f, 210.0f, 80.0f };
            Rectangle fsRec = { (float)uiRenderer.GetVirtualWidth() - 230.0f, 35.0f, 190.0f, 80.0f };

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
            float titleX = (float)uiRenderer.GetVirtualWidth() * 0.5f - 650.0f;
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

    // 3. State-Specific Progression
    switch (state) {
        case AppState::TITLE:
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                combatSystem.InitializeNewRun();
                state = AppState::BATTLE;
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                float titleX = (float)uiRenderer.GetVirtualWidth() * 0.5f - 650.0f;
                Rectangle startRec = { titleX + 1300.0f * 0.5f - 260.0f, 160.0f + 540.0f, 520.0f, 85.0f };
                if (CheckCollisionPointRec(mousePos, startRec)) {
                    combatSystem.InitializeNewRun();
                    state = AppState::BATTLE;
                }
            }
            break;

        case AppState::BATTLE:
            // Process Player Inputs only during PLAYER_INPUT phase
            if (combatSystem.GetPhase() == CombatPhase::PLAYER_INPUT) {
                // Keyboard Skill Selection: 1, 2, 3, 4
                if (IsKeyPressed(KEY_ONE)) combatSystem.SelectSkill(0);
                if (IsKeyPressed(KEY_TWO)) combatSystem.SelectSkill(1);
                if (IsKeyPressed(KEY_THREE)) combatSystem.SelectSkill(2);
                if (IsKeyPressed(KEY_FOUR)) combatSystem.SelectSkill(3);

                // Keyboard Stance Selection: Q (Attack), W (Defense), E (Parry)
                if (IsKeyPressed(KEY_Q)) combatSystem.SelectStance(StanceType::ATTACK);
                if (IsKeyPressed(KEY_W)) combatSystem.SelectStance(StanceType::DEFENSE);
                if (IsKeyPressed(KEY_E)) combatSystem.SelectStance(StanceType::PARRY);

                // Keyboard Execute Turn: SPACE or ENTER
                if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
                    combatSystem.ExecutePlayerTurn();
                }

                // Mouse Click Input Handling
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    // Stance Buttons Click
                    Rectangle atkRec = { GameConstants::STANCE_PANEL_X + 20.0f, GameConstants::STANCE_PANEL_Y + 65.0f, 180.0f, 170.0f };
                    Rectangle defRec = { GameConstants::STANCE_PANEL_X + 220.0f, GameConstants::STANCE_PANEL_Y + 65.0f, 180.0f, 170.0f };
                    Rectangle parRec = { GameConstants::STANCE_PANEL_X + 420.0f, GameConstants::STANCE_PANEL_Y + 65.0f, 180.0f, 170.0f };

                    if (CheckCollisionPointRec(mousePos, atkRec)) {
                        combatSystem.SelectStance(StanceType::ATTACK);
                    } else if (CheckCollisionPointRec(mousePos, defRec)) {
                        combatSystem.SelectStance(StanceType::DEFENSE);
                    } else if (CheckCollisionPointRec(mousePos, parRec)) {
                        combatSystem.SelectStance(StanceType::PARRY);
                    }

                    // Skill Cards Click
                    float spacing = 25.0f;
                    for (int i = 0; i < 4; ++i) {
                        Rectangle skillRec = { GameConstants::SKILL_TRAY_X + (float)i * (GameConstants::SKILL_CARD_W + spacing),
                                              GameConstants::SKILL_TRAY_Y, GameConstants::SKILL_CARD_W, GameConstants::SKILL_CARD_H };
                        if (CheckCollisionPointRec(mousePos, skillRec)) {
                            combatSystem.SelectSkill(i);
                        }
                    }

                    // Enemy Cards Click (Targeting)
                    const auto& enemies = combatSystem.GetEnemies();
                    float startX = GameConstants::ENEMY_START_X;
                    float cardWidth = 560.0f;
                    float enemySpacing = 35.0f;
                    if (enemies.size() == 2) {
                        startX = 900.0f;
                        cardWidth = 720.0f;
                        enemySpacing = 60.0f;
                    } else if (enemies.size() >= 3) {
                        startX = 720.0f;
                        cardWidth = 570.0f;
                        enemySpacing = 30.0f;
                    }
                    for (size_t i = 0; i < enemies.size(); ++i) {
                        if (enemies[i].IsAlive()) {
                            Rectangle enemyRec = { startX + (float)i * (cardWidth + enemySpacing), GameConstants::ENEMY_CARD_Y, cardWidth, GameConstants::ENEMY_CARD_H };
                            if (CheckCollisionPointRec(mousePos, enemyRec)) {
                                combatSystem.SelectTarget((int)i);
                            }
                        }
                    }

                    // Execute Button Click
                    Rectangle execRec = { GameConstants::EXECUTE_BTN_X, GameConstants::EXECUTE_BTN_Y, GameConstants::EXECUTE_BTN_W, GameConstants::EXECUTE_BTN_H };
                    if (CheckCollisionPointRec(mousePos, execRec)) {
                        combatSystem.ExecutePlayerTurn();
                    }
                }
            }

            // Update combat animations & state machine
            combatSystem.Update(dt);
            particleSystem.Update(dt, combatSystem.GetWeatherSystem().GetCurrentWeather());

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
            particleSystem.Update(dt, combatSystem.GetWeatherSystem().GetCurrentWeather());
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                if (combatSystem.GetCurrentWave() >= combatSystem.GetMaxWaves()) {
                    combatSystem.RestartGame();
                } else {
                    combatSystem.NextWave();
                }
                state = AppState::BATTLE;
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Rectangle nextRec = { (float)uiRenderer.GetVirtualWidth() * 0.5f - 340.0f, (float)uiRenderer.GetVirtualHeight() * 0.5f - 350.0f + 480.0f, 680.0f, 90.0f };
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
            particleSystem.Update(dt, combatSystem.GetWeatherSystem().GetCurrentWeather());
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                combatSystem.RestartGame();
                state = AppState::BATTLE;
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Rectangle retryRec = { (float)uiRenderer.GetVirtualWidth() * 0.5f - 260.0f, (float)uiRenderer.GetVirtualHeight() * 0.5f - 350.0f + 480.0f, 520.0f, 90.0f };
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
    uiRenderer.Render(combatSystem, particleSystem, state, selectedSettingIdx, showGuide, showSettings);
}
