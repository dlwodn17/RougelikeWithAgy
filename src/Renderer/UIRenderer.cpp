#include "Renderer/UIRenderer.hpp"

UIRenderer::UIRenderer(int virtWidth, int virtHeight)
    : virtualWidth(virtWidth), virtualHeight(virtHeight) {
}

void UIRenderer::Initialize() {
}

void UIRenderer::Render(const CombatSystem& combat, const ParticleSystem& particles, GameScene scene,
                        int selectedSettingIdx, bool showGuide, bool showSettings) const {
    if (scene == AppState::TITLE) {
        DrawTitleScreen();
    } else {
        // 1. Draw Combat Background
        CombatRenderer::DrawBackground(combat.GetWeatherSystem().GetCurrentWeather());

        // 2. Draw Particles (Weather & Effects)
        const_cast<ParticleSystem&>(particles).Draw();

        // 3. Draw Combat Arena Panels
        CombatRenderer::DrawWeatherForecast(combat.GetWeatherSystem());
        CombatRenderer::DrawPlayerPanel(combat.GetPlayer(), combat.GetSelectedStance());
        CombatRenderer::DrawEnemyPanel(combat);
        CombatRenderer::DrawStancePanel(combat);
        CombatRenderer::DrawSkillPanel(combat);
        CombatRenderer::DrawExecuteButton(combat);
        CombatRenderer::DrawLogPanel(combat.GetCombatLog());

        // 4. Overlays
        if (scene == AppState::VICTORY) {
            DrawVictoryOverlay(combat);
        } else if (scene == AppState::GAME_OVER) {
            DrawGameOverOverlay(combat);
        }
    }

    if (showGuide) {
        DrawGuideOverlay();
    }
    if (showSettings) {
        DrawSettingsOverlay(selectedSettingIdx);
    }
}

void UIRenderer::DrawTitleScreen() const {
    DrawRectangleGradientV(0, 0, virtualWidth, virtualHeight, (Color){ 12, 16, 26, 255 }, (Color){ 24, 30, 48, 255 });

    Rectangle frame = { (float)virtualWidth * 0.5f - 650.0f, 160.0f, 1300.0f, 1080.0f };
    CombatRenderer::DrawCard(frame, (Color){ 20, 26, 40, 240 }, (Color){ 241, 196, 15, 255 }, 0.05f);

    const char* title = "ELEMENTAL CONVERGENCE";
    int titleW = MeasureText(title, 64);
    DrawText(title, (int)(frame.x + (frame.width - titleW) * 0.5f), (int)frame.y + 70, 64, (Color){ 241, 196, 15, 255 });

    const char* subtitle = "Weather Forecast x Elemental Reaction Roguelike";
    int subW = MeasureText(subtitle, 26);
    DrawText(subtitle, (int)(frame.x + (frame.width - subW) * 0.5f), (int)frame.y + 150, 26, (Color){ 180, 195, 220, 255 });

    // Reaction Teasers
    Rectangle reactionBox = { frame.x + 80, frame.y + 215, frame.width - 160, 280 };
    DrawRectangleRounded(reactionBox, 0.08f, 6, (Color){ 14, 18, 28, 220 });
    DrawRectangleRoundedLinesEx(reactionBox, 0.08f, 6, 2.0f, (Color){ 65, 75, 95, 255 });

    DrawText("DISCOVER POWERFUL ELEMENTAL COMBOS:", (int)reactionBox.x + 25, (int)reactionBox.y + 20, 22, (Color){ 241, 196, 15, 255 });
    DrawText("• 💧 WET + ⚡ LIGHTNING = [SHOCK]    -> Electric chain damage across all enemies", (int)reactionBox.x + 30, (int)reactionBox.y + 65, 20, (Color){ 52, 152, 219, 255 });
    DrawText("• 🛢️ OIL + 🔥 FIRE = [EXPLOSION]     -> Cataclysmic blast + continuous Burning DoT", (int)reactionBox.x + 30, (int)reactionBox.y + 110, 20, (Color){ 231, 76, 60, 255 });
    DrawText("• 💧 WET + ❄️ COLD = [FROZEN]        -> Solid ice encasement skips enemy actions", (int)reactionBox.x + 30, (int)reactionBox.y + 155, 20, (Color){ 162, 222, 255, 255 });
    DrawText("• 🌪️ GALE WINDS (Weather)           -> Spreads active status debuffs to all units", (int)reactionBox.x + 30, (int)reactionBox.y + 200, 20, (Color){ 46, 204, 113, 255 });
    DrawText("• 🌦️ 3-Turn Forecast Ribbon        -> Plan ahead to amplify elemental synergies", (int)reactionBox.x + 30, (int)reactionBox.y + 240, 18, (Color){ 241, 196, 15, 255 });

    // Start Buttons
    Rectangle startRec = { frame.x + frame.width * 0.5f - 260.0f, frame.y + 540.0f, 520.0f, 85.0f };
    CombatRenderer::DrawButton(startRec, "⚔️ START EXPEDITION [ENTER / SPACE]", (Color){ 39, 174, 96, 255 }, (Color){ 46, 204, 113, 255 }, false, false, 24);

    Rectangle optRec = { frame.x + frame.width * 0.5f - 260.0f, frame.y + 640.0f, 520.0f, 75.0f };
    CombatRenderer::DrawButton(optRec, "⚙️ Video & Display Options [O]", (Color){ 41, 128, 185, 255 }, (Color){ 52, 152, 219, 255 }, false, false, 22);

    Rectangle fsRec = { frame.x + frame.width * 0.5f - 260.0f, frame.y + 730.0f, 520.0f, 75.0f };
    bool isFs = IsWindowFullscreen();
    std::string fsText = isFs ? "🗖 Windowed Mode" : "⛶ Fullscreen Mode [F11]";
    CombatRenderer::DrawButton(fsRec, fsText.c_str(), (Color){ 108, 92, 231, 255 }, (Color){ 155, 89, 182, 255 }, false, false, 22);

    DrawText("Native QHD (2560x1440) Vector Engine | Raylib 5.5 C++17 Modular Roguelike", (int)(frame.x + 300), (int)frame.y + 830, 20, (Color){ 140, 150, 170, 255 });
}

void UIRenderer::DrawVictoryOverlay(const CombatSystem& combat) const {
    DrawRectangle(0, 0, virtualWidth, virtualHeight, (Color){ 10, 25, 15, 200 });

    Rectangle modalRec = { (float)virtualWidth * 0.5f - 550.0f, (float)virtualHeight * 0.5f - 350.0f, 1100.0f, 700.0f };
    CombatRenderer::DrawCard(modalRec, (Color){ 20, 35, 28, 250 }, (Color){ 46, 204, 113, 255 }, 0.08f);

    const char* vicTitle = "★ VICTORY - WAVE CLEARED! ★";
    int tw = MeasureText(vicTitle, 46);
    DrawText(vicTitle, (int)(modalRec.x + (modalRec.width - tw) * 0.5f), (int)modalRec.y + 60, 46, (Color){ 46, 204, 113, 255 });

    std::string infoText = "Wave " + std::to_string(combat.GetCurrentWave()) + " of " + std::to_string(combat.GetMaxWaves()) + " Cleared!";
    int itw = MeasureText(infoText.c_str(), 28);
    DrawText(infoText.c_str(), (int)(modalRec.x + (modalRec.width - itw) * 0.5f), (int)modalRec.y + 130, 28, WHITE);

    DrawText("Hero Restored +25 Health & Cooldowns Reset", (int)modalRec.x + 240, (int)modalRec.y + 200, 24, (Color){ 241, 196, 15, 255 });

    std::string btnLabel = (combat.GetCurrentWave() >= combat.GetMaxWaves()) ? "★ ASCEND & PLAY AGAIN [SPACE / ENTER] ★" : "⚔️ ADVANCE TO NEXT WAVE [SPACE / ENTER]";
    Rectangle nextRec = { modalRec.x + modalRec.width * 0.5f - 340.0f, modalRec.y + 480.0f, 680.0f, 90.0f };
    CombatRenderer::DrawButton(nextRec, btnLabel.c_str(), (Color){ 39, 174, 96, 255 }, (Color){ 46, 204, 113, 255 }, false, false, 24);
}

void UIRenderer::DrawGameOverOverlay(const CombatSystem& combat) const {
    (void)combat;
    DrawRectangle(0, 0, virtualWidth, virtualHeight, (Color){ 25, 10, 10, 210 });

    Rectangle modalRec = { (float)virtualWidth * 0.5f - 550.0f, (float)virtualHeight * 0.5f - 350.0f, 1100.0f, 700.0f };
    CombatRenderer::DrawCard(modalRec, (Color){ 35, 20, 20, 250 }, (Color){ 231, 76, 60, 255 }, 0.08f);

    const char* defTitle = "☠ DEFEAT - RUN CONCLUDED ☠";
    int tw = MeasureText(defTitle, 46);
    DrawText(defTitle, (int)(modalRec.x + (modalRec.width - tw) * 0.5f), (int)modalRec.y + 60, 46, (Color){ 231, 76, 60, 255 });

    const char* sub = "The elemental convergence overwhelmed your defenses.";
    int stw = MeasureText(sub, 24);
    DrawText(sub, (int)(modalRec.x + (modalRec.width - stw) * 0.5f), (int)modalRec.y + 130, 24, (Color){ 200, 200, 210, 255 });

    DrawText("Tip: Utilize [Defense Stance] (+18 Shield) and [Parry] to counter devastating bursts!", (int)modalRec.x + 90, (int)modalRec.y + 220, 22, (Color){ 241, 196, 15, 255 });

    Rectangle retryRec = { modalRec.x + modalRec.width * 0.5f - 260.0f, modalRec.y + 480.0f, 520.0f, 90.0f };
    CombatRenderer::DrawButton(retryRec, "🔄 RETRY RUN [SPACE / ENTER]", (Color){ 192, 57, 43, 255 }, (Color){ 231, 76, 60, 255 }, false, false, 24);
}

void UIRenderer::DrawSettingsOverlay(int selectedIdx) const {
    DrawRectangle(0, 0, virtualWidth, virtualHeight, (Color){ 10, 12, 18, 235 });

    Rectangle modalRec = { (float)virtualWidth * 0.5f - 750.0f, (float)virtualHeight * 0.5f - 500.0f, 1500.0f, 1000.0f };
    CombatRenderer::DrawCard(modalRec, (Color){ 24, 30, 45, 255 }, (Color){ 241, 196, 15, 255 }, 0.06f);

    DrawText("⚙️ VIDEO & DISPLAY RESOLUTION OPTIONS", (int)modalRec.x + 50, (int)modalRec.y + 45, 34, (Color){ 241, 196, 15, 255 });
    DrawText("Navigate with [W/S] or [Up/Down], change with [A/D] or [Left/Right], or click directly.", (int)modalRec.x + 50, (int)modalRec.y + 90, 20, (Color){ 180, 190, 210, 255 });

    // 1. Resolution Options Section
    float y = modalRec.y + 145.0f;
    bool row0Selected = (selectedIdx == 0);
    DrawText("1. OUTPUT RESOLUTION (UP TO QHD 2560x1440):", (int)modalRec.x + 50, (int)y, 24, row0Selected ? (Color){ 241, 196, 15, 255 } : WHITE);
    y += 45.0f;

    const auto& resList = DisplaySettings::GetResolutions();
    int currentResIdx = DisplaySettings::GetCurrentResolutionIndex();

    for (size_t i = 0; i < resList.size(); ++i) {
        Rectangle btnRec = { modalRec.x + 60.0f + (float)i * 340.0f, y, 320.0f, 100.0f };
        bool isActive = (static_cast<int>(i) == currentResIdx);

        std::string btnText = std::string(resList[i].label) + "\n\n" + resList[i].tag;
        CombatRenderer::DrawButton(btnRec, btnText.c_str(), (Color){ 30, 38, 55, 255 }, (Color){ 41, 128, 185, 255 }, isActive, false, 20);
    }

    y += 140.0f;

    // 2. Display Mode Section
    bool row1Selected = (selectedIdx == 1);
    DrawText("2. DISPLAY MODE:", (int)modalRec.x + 50, (int)y, 24, row1Selected ? (Color){ 241, 196, 15, 255 } : WHITE);
    y += 45.0f;

    bool isFs = IsWindowFullscreen();
    Rectangle winBtnRec = { modalRec.x + 60.0f, y, 400.0f, 90.0f };
    Rectangle fsBtnRec = { modalRec.x + 490.0f, y, 400.0f, 90.0f };

    CombatRenderer::DrawButton(winBtnRec, "🗖 Windowed Mode", (Color){ 30, 38, 55, 255 }, (Color){ 52, 152, 219, 255 }, !isFs, false, 22);
    CombatRenderer::DrawButton(fsBtnRec, "⛶ Fullscreen Mode [F11]", (Color){ 30, 38, 55, 255 }, (Color){ 108, 92, 231, 255 }, isFs, false, 22);

    y += 135.0f;

    // 3. Hardware Specs
    Rectangle infoRec = { modalRec.x + 50.0f, y, modalRec.width - 100.0f, 240.0f };
    DrawRectangleRounded(infoRec, 0.08f, 6, (Color){ 18, 22, 34, 230 });
    DrawRectangleRoundedLinesEx(infoRec, 0.08f, 6, 2.0f, (Color){ 65, 75, 95, 255 });

    int monitor = GetCurrentMonitor();
    int monW = GetMonitorWidth(monitor);
    int monH = GetMonitorHeight(monitor);

    DrawText("GRAPHICS & HARDWARE SPECIFICATIONS:", (int)infoRec.x + 25, (int)infoRec.y + 20, 20, (Color){ 241, 196, 15, 255 });
    
    std::string internalRes = "• Internal Render Target Canvas: 2560 x 1440 (Native QHD High-DPI Vector Buffer)";
    std::string scalingMode = "• Scaling Pipeline: Hardware Bilinear Filtering + 16:9 Letterboxing Filter";
    std::string monInfo = "• Primary Monitor Native Resolution: " + std::to_string(monW) + " x " + std::to_string(monH);
    std::string currentWin = "• Current Window / Screen Output: " + std::to_string(GetScreenWidth()) + " x " + std::to_string(GetScreenHeight()) + (isFs ? " (Fullscreen)" : " (Windowed)");

    DrawText(internalRes.c_str(), (int)infoRec.x + 25, (int)infoRec.y + 60, 18, (Color){ 190, 200, 220, 255 });
    DrawText(scalingMode.c_str(), (int)infoRec.x + 25, (int)infoRec.y + 95, 18, (Color){ 190, 200, 220, 255 });
    DrawText(monInfo.c_str(), (int)infoRec.x + 25, (int)infoRec.y + 130, 18, (Color){ 190, 200, 220, 255 });
    DrawText(currentWin.c_str(), (int)infoRec.x + 25, (int)infoRec.y + 165, 18, (Color){ 46, 204, 113, 255 });

    // Close Button
    bool row2Selected = (selectedIdx >= 2);
    Rectangle closeRec = { modalRec.x + modalRec.width * 0.5f - 180.0f, modalRec.y + modalRec.height - 90.0f, 360.0f, 65.0f };
    CombatRenderer::DrawButton(closeRec, "Apply & Close [ESC / O]", (Color){ 39, 174, 96, 255 }, (Color){ 46, 204, 113, 255 }, row2Selected, false, 22);
}

void UIRenderer::DrawGuideOverlay() const {
    DrawRectangle(0, 0, virtualWidth, virtualHeight, (Color){ 10, 12, 18, 230 });

    Rectangle modalRec = { (float)virtualWidth * 0.5f - 700.0f, (float)virtualHeight * 0.5f - 480.0f, 1400.0f, 960.0f };
    CombatRenderer::DrawCard(modalRec, (Color){ 24, 30, 45, 255 }, (Color){ 241, 196, 15, 255 }, 0.06f);

    DrawText("★ ELEMENTAL REACTION & WEATHER COMPENDIUM", (int)modalRec.x + 50, (int)modalRec.y + 45, 34, (Color){ 241, 196, 15, 255 });

    float y = modalRec.y + 110.0f;
    DrawText("CORE ELEMENTAL REACTIONS:", (int)modalRec.x + 50, (int)y, 24, WHITE);
    y += 40.0f;

    DrawText("• 💧 WET + ⚡ LIGHTNING = [SHOCK]    -> Massive burst + Arcs AoE chain damage to all alive enemies!", (int)modalRec.x + 60, (int)y, 20, (Color){ 241, 196, 15, 255 });
    y += 36.0f;
    DrawText("• 🛢️ OIL + 🔥 FIRE = [EXPLOSION]     -> Cataclysmic burst damage + inflicts Burning DoT to target!", (int)modalRec.x + 60, (int)y, 20, (Color){ 231, 76, 60, 255 });
    y += 36.0f;
    DrawText("• 💧 WET + ❄️ COLD = [FROZEN]        -> Immobilizes target completely; skips their next turn!", (int)modalRec.x + 60, (int)y, 20, (Color){ 162, 222, 255, 255 });
    y += 36.0f;
    DrawText("• 🔥 FIRE + ❄️ COLD = [MELT]         -> Superheated steam vaporization bonus damage.", (int)modalRec.x + 60, (int)y, 20, (Color){ 243, 156, 18, 255 });
    y += 36.0f;
    DrawText("• 🛢️ OIL + ⚡ LIGHTNING = [PLASMA]    -> Piercing discharge bonus damage bypassing armor.", (int)modalRec.x + 60, (int)y, 20, (Color){ 155, 89, 182, 255 });
    y += 50.0f;

    DrawText("DYNAMIC WEATHER FORECAST MECHANICS:", (int)modalRec.x + 50, (int)y, 24, WHITE);
    y += 40.0f;
    DrawText("• 🌧️ Rain: Applies [WET] globally each turn | Boosts Water DMG +35%", (int)modalRec.x + 60, (int)y, 20, (Color){ 52, 152, 219, 255 });
    y += 36.0f;
    DrawText("• 🔥 Heatwave: Extreme dry heat | Boosts Fire DMG +50%", (int)modalRec.x + 60, (int)y, 20, (Color){ 230, 126, 34, 255 });
    y += 36.0f;
    DrawText("• ⛈️ Thunderstorm: Applies [WET] globally + Calls down 15 DMG Lightning Strikes!", (int)modalRec.x + 60, (int)y, 20, (Color){ 142, 68, 173, 255 });
    y += 36.0f;
    DrawText("• 🌨️ Blizzard: Freezes all [WET] targets instantly | Boosts Cold DMG +30%", (int)modalRec.x + 60, (int)y, 20, (Color){ 129, 236, 236, 255 });
    y += 36.0f;
    DrawText("• 🌪️ Gale Winds: Swirls & spreads active elemental debuffs across all enemies!", (int)modalRec.x + 60, (int)y, 20, (Color){ 46, 204, 113, 255 });
    y += 36.0f;
    DrawText("• 🧪 Acid Rain: Coats all units in combustible [OIL]!", (int)modalRec.x + 60, (int)y, 20, (Color){ 108, 92, 231, 255 });

    Rectangle closeRec = { modalRec.x + modalRec.width * 0.5f - 140.0f, modalRec.y + modalRec.height - 90.0f, 280.0f, 60.0f };
    CombatRenderer::DrawButton(closeRec, "Close Guide [ESC]", (Color){ 39, 174, 96, 255 }, (Color){ 46, 204, 113, 255 }, false, false, 22);
}
