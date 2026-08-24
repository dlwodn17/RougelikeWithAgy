#include "GameState.hpp"
#include "Renderer/FontManager.hpp"
#include "Core/Localization.hpp"

GameManager::GameManager() 
    : state(AppState::TITLE), showSettings(false), showGuide(false), selectedSettingIdx(0) {
}

void GameManager::Initialize() {
    FontManager::Initialize();
    uiRenderer.Initialize();
    rewardSystem.InitializeRunePool();
    combatSystem.SetParticleSystem(&particleSystem);
    combatSystem.InitializeNewRun();
    mapSystem.GenerateNewMap();
}

void GameManager::Update(float dt) {
    Vector2 mousePos = ScreenConfig::GetVirtualMousePosition();

    // Global Language Toggle Hotkey [L]
    if (IsKeyPressed(KEY_L)) {
        Localization::ToggleLanguage();
    }

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
        } else if (selectedSettingIdx == 1) { // Display Mode / Language Row
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
            float yRes = modalY + 182.0f;
            for (int i = 0; i < 4; ++i) {
                Rectangle btnRec = { modalX + 60.0f + (float)i * 340.0f, yRes, 320.0f, 95.0f };
                if (CheckCollisionPointRec(mousePos, btnRec)) {
                    DisplaySettings::SetResolutionIndex(i);
                    selectedSettingIdx = 0;
                    return;
                }
            }

            // Display mode & language buttons (row 1)
            float yMode = modalY + 357.0f;
            Rectangle winBtnRec = { modalX + 60.0f, yMode, 320.0f, 85.0f };
            Rectangle fsBtnRec = { modalX + 400.0f, yMode, 320.0f, 85.0f };
            Rectangle langBtnRec = { modalX + 740.0f, yMode, 640.0f, 85.0f };

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
            if (CheckCollisionPointRec(mousePos, langBtnRec)) {
                Localization::ToggleLanguage();
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

    // Check Header Button Clicks during BATTLE, TITLE, or MAP
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if (state == AppState::BATTLE || state == AppState::MAP_NAVIGATION) {
            Rectangle langRec = { (float)uiRenderer.GetVirtualWidth() - 870.0f, 35.0f, 170.0f, 80.0f };
            Rectangle helpRec = { (float)uiRenderer.GetVirtualWidth() - 680.0f, 35.0f, 200.0f, 80.0f };
            Rectangle optRec = { (float)uiRenderer.GetVirtualWidth() - 460.0f, 35.0f, 210.0f, 80.0f };
            Rectangle fsRec = { (float)uiRenderer.GetVirtualWidth() - 230.0f, 35.0f, 190.0f, 80.0f };

            if (CheckCollisionPointRec(mousePos, langRec)) {
                Localization::ToggleLanguage();
                return;
            }
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
            Rectangle optRec = { titleX + 1300.0f * 0.5f - 260.0f, 160.0f + 635.0f, 520.0f, 75.0f };
            Rectangle fsRec = { titleX + 1300.0f * 0.5f - 260.0f, 160.0f + 725.0f, 520.0f, 75.0f };
            Rectangle langRec = { titleX + 1300.0f * 0.5f - 260.0f, 160.0f + 815.0f, 520.0f, 65.0f };

            if (CheckCollisionPointRec(mousePos, optRec)) {
                showSettings = true;
                return;
            }
            if (CheckCollisionPointRec(mousePos, fsRec)) {
                DisplaySettings::ToggleFullscreenMode();
                return;
            }
            if (CheckCollisionPointRec(mousePos, langRec)) {
                Localization::ToggleLanguage();
                return;
            }
        }
    }

    // 3. State-Specific Progression
    switch (state) {
        case AppState::TITLE:
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                rewardSystem.Reset();
                combatSystem.InitializeNewRun();
                mapSystem.GenerateNewMap();
                state = AppState::MAP_NAVIGATION;
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                float titleX = (float)uiRenderer.GetVirtualWidth() * 0.5f - 650.0f;
                Rectangle startRec = { titleX + 1300.0f * 0.5f - 260.0f, 160.0f + 535.0f, 520.0f, 85.0f };
                if (CheckCollisionPointRec(mousePos, startRec)) {
                    rewardSystem.Reset();
                    combatSystem.InitializeNewRun();
                    mapSystem.GenerateNewMap();
                    state = AppState::MAP_NAVIGATION;
                }
            }
            break;

        case AppState::MAP_NAVIGATION:
            mapSystem.Update(dt);
            particleSystem.Update(dt, combatSystem.GetWeatherSystem().GetCurrentWeather());

            // Node Selection Input
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                const auto& nodes = mapSystem.GetAllNodes();
                for (const auto& node : nodes) {
                    if (node.isAvailable && !node.isEroded) {
                        Vector2 center = { node.posX, node.posY };
                        if (CheckCollisionPointCircle(mousePos, center, node.radius)) {
                            int targetId = node.id;
                            NodeType nType = node.type;
                            int nLayer = node.layer;
                            int nRisk = node.erosionRisk;

                            mapSystem.MoveToNode(targetId);

                            if (nType == NodeType::COMBAT || nType == NodeType::ELITE || nType == NodeType::BOSS) {
                                combatSystem.StartNodeBattle(nType, nLayer, nRisk);
                                state = AppState::BATTLE;
                            } else if (nType == NodeType::REST_SITE) {
                                state = AppState::REST_SCREEN;
                            } else if (nType == NodeType::WEATHER_SHRINE) {
                                state = AppState::SHRINE_SCREEN;
                            }
                            return;
                        }
                    }
                }
            }
            break;

        case AppState::REST_SCREEN:
            particleSystem.Update(dt, combatSystem.GetWeatherSystem().GetCurrentWeather());

            // Hotkeys: [1] Heal, [2] Cleanse / Meditate
            if (IsKeyPressed(KEY_ONE)) {
                mapSystem.PerformRestHeal(combatSystem.GetPlayer());
                state = AppState::MAP_NAVIGATION;
                return;
            }
            if (IsKeyPressed(KEY_TWO)) {
                mapSystem.PerformRestCleanse(combatSystem.GetSkillSystem(), 0);
                state = AppState::MAP_NAVIGATION;
                return;
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                float w = (float)uiRenderer.GetVirtualWidth();
                float cardW = 660.0f;
                float cardH = 680.0f;
                float startX = (w - (2.0f * cardW + 80.0f)) * 0.5f;
                float startY = 320.0f;

                Rectangle card1Rec = { startX, startY, cardW, cardH };
                Rectangle card2Rec = { startX + cardW + 80.0f, startY, cardW, cardH };

                if (CheckCollisionPointRec(mousePos, card1Rec)) {
                    mapSystem.PerformRestHeal(combatSystem.GetPlayer());
                    state = AppState::MAP_NAVIGATION;
                    return;
                }
                if (CheckCollisionPointRec(mousePos, card2Rec)) {
                    mapSystem.PerformRestCleanse(combatSystem.GetSkillSystem(), 0);
                    state = AppState::MAP_NAVIGATION;
                    return;
                }
            }
            break;

        case AppState::SHRINE_SCREEN:
            particleSystem.Update(dt, combatSystem.GetWeatherSystem().GetCurrentWeather());

            // Hotkeys: [1] Rain, [2] Heatwave, [3] Blizzard, [4] Storm
            if (IsKeyPressed(KEY_ONE)) {
                mapSystem.PerformShrineWeatherSet(combatSystem.GetWeatherSystem(), WeatherType::RAIN);
                state = AppState::MAP_NAVIGATION;
                return;
            }
            if (IsKeyPressed(KEY_TWO)) {
                mapSystem.PerformShrineWeatherSet(combatSystem.GetWeatherSystem(), WeatherType::HEATWAVE);
                state = AppState::MAP_NAVIGATION;
                return;
            }
            if (IsKeyPressed(KEY_THREE)) {
                mapSystem.PerformShrineWeatherSet(combatSystem.GetWeatherSystem(), WeatherType::BLIZZARD);
                state = AppState::MAP_NAVIGATION;
                return;
            }
            if (IsKeyPressed(KEY_FOUR)) {
                mapSystem.PerformShrineWeatherSet(combatSystem.GetWeatherSystem(), WeatherType::STORM);
                state = AppState::MAP_NAVIGATION;
                return;
            }

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                float w = (float)uiRenderer.GetVirtualWidth();
                float cardW = 540.0f;
                float cardH = 820.0f;
                float spacing = 35.0f;
                float totalW = 4.0f * cardW + 3.0f * spacing;
                float startX = (w - totalW) * 0.5f;
                float startY = 260.0f;

                WeatherType weathers[4] = { WeatherType::RAIN, WeatherType::HEATWAVE, WeatherType::BLIZZARD, WeatherType::STORM };

                for (int i = 0; i < 4; ++i) {
                    Rectangle cardRec = { startX + (float)i * (cardW + spacing), startY, cardW, cardH };
                    if (CheckCollisionPointRec(mousePos, cardRec)) {
                        mapSystem.PerformShrineWeatherSet(combatSystem.GetWeatherSystem(), weathers[i]);
                        state = AppState::MAP_NAVIGATION;
                        return;
                    }
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
                    Rectangle atkRec = { GameConstants::STANCE_PANEL_X + 20.0f, GameConstants::STANCE_PANEL_Y + 60.0f, 180.0f, 175.0f };
                    Rectangle defRec = { GameConstants::STANCE_PANEL_X + 220.0f, GameConstants::STANCE_PANEL_Y + 60.0f, 180.0f, 175.0f };
                    Rectangle parRec = { GameConstants::STANCE_PANEL_X + 420.0f, GameConstants::STANCE_PANEL_Y + 60.0f, 180.0f, 175.0f };

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
                rewardSystem.GenerateRewardRunes(3);
                state = AppState::REWARD_SCREEN;
            } else if (combatSystem.GetPhase() == CombatPhase::DEFEAT_SCREEN) {
                state = AppState::GAME_OVER;
            }

            // Quick restart hotkey
            if (IsKeyPressed(KEY_R)) {
                rewardSystem.Reset();
                combatSystem.RestartGame();
                mapSystem.GenerateNewMap();
                state = AppState::MAP_NAVIGATION;
            }
            break;

        case AppState::REWARD_SCREEN:
            particleSystem.Update(dt, combatSystem.GetWeatherSystem().GetCurrentWeather());

            if (!rewardSystem.IsSocketingPopupOpen()) {
                // Keyboard 1, 2, 3 selection
                if (IsKeyPressed(KEY_ONE)) rewardSystem.SelectRune(0);
                if (IsKeyPressed(KEY_TWO)) rewardSystem.SelectRune(1);
                if (IsKeyPressed(KEY_THREE)) rewardSystem.SelectRune(2);

                // Mouse click on 3 rune cards
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    float cardW = 680.0f;
                    float cardH = 960.0f;
                    float spacing = 50.0f;
                    float totalW = 3 * cardW + 2 * spacing;
                    float startX = ((float)uiRenderer.GetVirtualWidth() - totalW) * 0.5f;
                    float startY = 260.0f;
                    for (int i = 0; i < 3; ++i) {
                        Rectangle cardRec = { startX + (float)i * (cardW + spacing), startY, cardW, cardH };
                        if (CheckCollisionPointRec(mousePos, cardRec)) {
                            rewardSystem.SelectRune(i);
                            break;
                        }
                    }
                }
            } else {
                // Socketing popup is active
                if (IsKeyPressed(KEY_ESCAPE)) {
                    rewardSystem.CancelSelection();
                }
                auto advancePostReward = [&]() {
                    if (combatSystem.GetCurrentEncounterType() == NodeType::BOSS) {
                        state = AppState::VICTORY;
                    } else {
                        state = AppState::MAP_NAVIGATION;
                    }
                };

                if (IsKeyPressed(KEY_ONE)) {
                    rewardSystem.SocketRuneToSkill(combatSystem.GetSkillSystem(), 0);
                    advancePostReward();
                    return;
                }
                if (IsKeyPressed(KEY_TWO)) {
                    rewardSystem.SocketRuneToSkill(combatSystem.GetSkillSystem(), 1);
                    advancePostReward();
                    return;
                }
                if (IsKeyPressed(KEY_THREE)) {
                    rewardSystem.SocketRuneToSkill(combatSystem.GetSkillSystem(), 2);
                    advancePostReward();
                    return;
                }
                if (IsKeyPressed(KEY_FOUR)) {
                    rewardSystem.SocketRuneToSkill(combatSystem.GetSkillSystem(), 3);
                    advancePostReward();
                    return;
                }

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    float modalX = (float)uiRenderer.GetVirtualWidth() * 0.5f - 850.0f;
                    float modalY = (float)uiRenderer.GetVirtualHeight() * 0.5f - 460.0f;
                    float skillCardW = 370.0f;
                    float skillCardH = 460.0f;
                    float skillSpacing = 25.0f;
                    float startX = modalX + 60.0f;
                    float startY = modalY + 275.0f;

                    // Click on one of 4 skill cards
                    for (int i = 0; i < 4; ++i) {
                        Rectangle scRec = { startX + (float)i * (skillCardW + skillSpacing), startY, skillCardW, skillCardH };
                        if (CheckCollisionPointRec(mousePos, scRec)) {
                            rewardSystem.SocketRuneToSkill(combatSystem.GetSkillSystem(), i);
                            advancePostReward();
                            return;
                        }
                    }

                    // Click Cancel button
                    Rectangle cancelRec = { modalX + 1700.0f * 0.5f - 180.0f, modalY + 920.0f - 70.0f, 360.0f, 50.0f };
                    if (CheckCollisionPointRec(mousePos, cancelRec)) {
                        rewardSystem.CancelSelection();
                        return;
                    }
                }
            }
            break;

        case AppState::VICTORY:
            particleSystem.Update(dt, combatSystem.GetWeatherSystem().GetCurrentWeather());
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                rewardSystem.Reset();
                combatSystem.RestartGame();
                mapSystem.GenerateNewMap();
                state = AppState::MAP_NAVIGATION;
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Rectangle nextRec = { (float)uiRenderer.GetVirtualWidth() * 0.5f - 340.0f, (float)uiRenderer.GetVirtualHeight() * 0.5f - 350.0f + 480.0f, 680.0f, 90.0f };
                if (CheckCollisionPointRec(mousePos, nextRec)) {
                    rewardSystem.Reset();
                    combatSystem.RestartGame();
                    mapSystem.GenerateNewMap();
                    state = AppState::MAP_NAVIGATION;
                }
            }
            break;

        case AppState::GAME_OVER:
            particleSystem.Update(dt, combatSystem.GetWeatherSystem().GetCurrentWeather());
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                rewardSystem.Reset();
                combatSystem.RestartGame();
                mapSystem.GenerateNewMap();
                state = AppState::MAP_NAVIGATION;
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Rectangle retryRec = { (float)uiRenderer.GetVirtualWidth() * 0.5f - 260.0f, (float)uiRenderer.GetVirtualHeight() * 0.5f - 350.0f + 480.0f, 520.0f, 90.0f };
                if (CheckCollisionPointRec(mousePos, retryRec)) {
                    rewardSystem.Reset();
                    combatSystem.RestartGame();
                    mapSystem.GenerateNewMap();
                    state = AppState::MAP_NAVIGATION;
                }
            }
            break;

        default:
            break;
    }
}

void GameManager::Draw() {
    if (state == AppState::MAP_NAVIGATION) {
        MapRenderer::DrawMapScreen(mapSystem, combatSystem.GetPlayer(), combatSystem.GetWeatherSystem());
        particleSystem.Draw();
        if (showGuide) uiRenderer.DrawGuideOverlay();
        if (showSettings) uiRenderer.DrawSettingsOverlay(selectedSettingIdx);
    } else if (state == AppState::REST_SCREEN) {
        MapRenderer::DrawRestScreen(combatSystem.GetPlayer(), combatSystem.GetSkillSystem());
        particleSystem.Draw();
        if (showGuide) uiRenderer.DrawGuideOverlay();
        if (showSettings) uiRenderer.DrawSettingsOverlay(selectedSettingIdx);
    } else if (state == AppState::SHRINE_SCREEN) {
        MapRenderer::DrawShrineScreen(combatSystem.GetWeatherSystem());
        particleSystem.Draw();
        if (showGuide) uiRenderer.DrawGuideOverlay();
        if (showSettings) uiRenderer.DrawSettingsOverlay(selectedSettingIdx);
    } else if (state == AppState::REWARD_SCREEN) {
        CombatRenderer::DrawBackground(combatSystem.GetWeatherSystem().GetCurrentWeather());
        particleSystem.Draw();
        RewardRenderer::DrawRewardScreen(rewardSystem, combatSystem.GetSkillSystem());
        if (showGuide) uiRenderer.DrawGuideOverlay();
        if (showSettings) uiRenderer.DrawSettingsOverlay(selectedSettingIdx);
    } else {
        uiRenderer.Render(combatSystem, particleSystem, state, selectedSettingIdx, showGuide, showSettings);
    }
}
