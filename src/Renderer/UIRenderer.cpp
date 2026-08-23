#include "Renderer/UIRenderer.hpp"
#include "Renderer/FontManager.hpp"
#include "Core/Localization.hpp"

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

    const char* title = Localization::IsKorean() ? "원소 수렴의 탑 (ELEMENTAL CONVERGENCE)" : "ELEMENTAL CONVERGENCE";
    int titleW = FontManager::MeasureText(title, FontSize::TITLE_LARGE);
    FontManager::DrawText(title, (int)(frame.x + (frame.width - titleW) * 0.5f), (int)frame.y + 60, FontSize::TITLE_LARGE, (Color){ 241, 196, 15, 255 });

    const char* subtitle = Localization::IsKorean() ? "날씨 예보 x 원소 반응 턴제 로그라이크" : "Weather Forecast x Elemental Reaction Roguelike";
    int subW = FontManager::MeasureText(subtitle, FontSize::CARD_TITLE);
    FontManager::DrawText(subtitle, (int)(frame.x + (frame.width - subW) * 0.5f), (int)frame.y + 145, FontSize::CARD_TITLE, (Color){ 180, 195, 220, 255 });

    // Reaction Teasers
    Rectangle reactionBox = { frame.x + 80, frame.y + 205, frame.width - 160, 300 };
    DrawRectangleRounded(reactionBox, 0.08f, 6, (Color){ 14, 18, 28, 220 });
    DrawRectangleRoundedLinesEx(reactionBox, 0.08f, 6, 2.0f, (Color){ 65, 75, 95, 255 });

    if (Localization::IsKorean()) {
        FontManager::DrawText("강력한 원소 시너지 & 날씨 반응 조합:", (int)reactionBox.x + 25, (int)reactionBox.y + 18, FontSize::BUTTON_MEDIUM + 2, (Color){ 241, 196, 15, 255 });
        FontManager::DrawText("- [수분] + [전기] = [감전 (SHOCK)]     -> 폭발적인 번개 아크가 모든 적에게 연쇄 전파", (int)reactionBox.x + 30, (int)reactionBox.y + 60, FontSize::BODY_REGULAR, (Color){ 52, 152, 219, 255 });
        FontManager::DrawText("- [기름] + [화염] = [폭발 (EXPLOSION)] -> 대규모 폭발 피해 + 지속 화상(DoT) 부여", (int)reactionBox.x + 30, (int)reactionBox.y + 105, FontSize::BODY_REGULAR, (Color){ 231, 76, 60, 255 });
        FontManager::DrawText("- [수분] + [냉기] = [빙결 (FROZEN)]    -> 대상을 완전히 얼려 다음 턴 행동 불가", (int)reactionBox.x + 30, (int)reactionBox.y + 150, FontSize::BODY_REGULAR, (Color){ 162, 222, 255, 255 });
        FontManager::DrawText("- [돌풍 (GALE)] 날씨 환경 효과         -> 모든 적에게 활성화된 상태이상을 광역 확산", (int)reactionBox.x + 30, (int)reactionBox.y + 195, FontSize::BODY_REGULAR, (Color){ 46, 204, 113, 255 });
        FontManager::DrawText("- 3턴 날씨 예보 큐 시스템              -> 다가올 날씨에 맞춰 최적의 스킬 연계 전략 수립", (int)reactionBox.x + 30, (int)reactionBox.y + 240, FontSize::BODY_SMALL, (Color){ 241, 196, 15, 255 });
    } else {
        FontManager::DrawText("DISCOVER POWERFUL ELEMENTAL COMBOS:", (int)reactionBox.x + 25, (int)reactionBox.y + 18, FontSize::BUTTON_MEDIUM + 2, (Color){ 241, 196, 15, 255 });
        FontManager::DrawText("- [WET] + [ELEC] = [SHOCK]      -> Electric chain damage across all enemies", (int)reactionBox.x + 30, (int)reactionBox.y + 60, FontSize::BODY_REGULAR, (Color){ 52, 152, 219, 255 });
        FontManager::DrawText("- [OIL] + [FIRE] = [EXPLOSION]  -> Cataclysmic blast + continuous Burning DoT", (int)reactionBox.x + 30, (int)reactionBox.y + 105, FontSize::BODY_REGULAR, (Color){ 231, 76, 60, 255 });
        FontManager::DrawText("- [WET] + [COLD] = [FROZEN]     -> Solid ice encasement skips enemy actions", (int)reactionBox.x + 30, (int)reactionBox.y + 150, FontSize::BODY_REGULAR, (Color){ 162, 222, 255, 255 });
        FontManager::DrawText("- [GALE] WINDS (Weather)        -> Spreads active status debuffs to all units", (int)reactionBox.x + 30, (int)reactionBox.y + 195, FontSize::BODY_REGULAR, (Color){ 46, 204, 113, 255 });
        FontManager::DrawText("- 3-Turn Forecast Ribbon        -> Plan ahead to amplify elemental synergies", (int)reactionBox.x + 30, (int)reactionBox.y + 240, FontSize::BODY_SMALL, (Color){ 241, 196, 15, 255 });
    }

    // Start Buttons
    Rectangle startRec = { frame.x + frame.width * 0.5f - 260.0f, frame.y + 535.0f, 520.0f, 85.0f };
    const char* startLabel = Localization::IsKorean() ? "원정 시작 [ENTER / SPACE]" : "START EXPEDITION [ENTER / SPACE]";
    CombatRenderer::DrawButton(startRec, startLabel, (Color){ 39, 174, 96, 255 }, (Color){ 46, 204, 113, 255 }, false, false, FontSize::BUTTON_LARGE);

    Rectangle optRec = { frame.x + frame.width * 0.5f - 260.0f, frame.y + 635.0f, 520.0f, 75.0f };
    const char* optLabel = Localization::IsKorean() ? "해상도 및 언어 설정 [O]" : "Video & Language Options [O]";
    CombatRenderer::DrawButton(optRec, optLabel, (Color){ 41, 128, 185, 255 }, (Color){ 52, 152, 219, 255 }, false, false, FontSize::BUTTON_MEDIUM + 2);

    Rectangle fsRec = { frame.x + frame.width * 0.5f - 260.0f, frame.y + 725.0f, 520.0f, 75.0f };
    bool isFs = IsWindowFullscreen();
    std::string fsText = isFs ? (Localization::IsKorean() ? "창모드 전환" : "Windowed Mode") : (Localization::IsKorean() ? "전체화면 모드 [F11]" : "Fullscreen Mode [F11]");
    CombatRenderer::DrawButton(fsRec, fsText.c_str(), (Color){ 108, 92, 231, 255 }, (Color){ 155, 89, 182, 255 }, false, false, FontSize::BUTTON_MEDIUM + 2);

    // Language Toggle on Title Screen
    Rectangle langRec = { frame.x + frame.width * 0.5f - 260.0f, frame.y + 815.0f, 520.0f, 65.0f };
    std::string langTitle = Localization::IsKorean() ? "언어 전환: 한국어 [L]" : "Language Toggle: English [L]";
    CombatRenderer::DrawButton(langRec, langTitle.c_str(), (Color){ 230, 126, 34, 255 }, (Color){ 243, 156, 18, 255 }, false, false, FontSize::BUTTON_MEDIUM);

    FontManager::DrawText("Native QHD (2560x1440) Vector Engine | Raylib 5.5 C++17 Modular Roguelike", (int)(frame.x + 300), (int)frame.y + 910, FontSize::BODY_REGULAR, (Color){ 140, 150, 170, 255 });
}

void UIRenderer::DrawVictoryOverlay(const CombatSystem& combat) const {
    DrawRectangle(0, 0, virtualWidth, virtualHeight, (Color){ 10, 25, 15, 200 });

    Rectangle modalRec = { (float)virtualWidth * 0.5f - 550.0f, (float)virtualHeight * 0.5f - 350.0f, 1100.0f, 700.0f };
    CombatRenderer::DrawCard(modalRec, (Color){ 20, 35, 28, 250 }, (Color){ 46, 204, 113, 255 }, 0.08f);

    const char* vicTitle = Localization::IsKorean() ? "★ 승리 - 웨이브 돌파! ★" : "* VICTORY - WAVE CLEARED! *";
    int tw = FontManager::MeasureText(vicTitle, FontSize::TITLE_MEDIUM);
    FontManager::DrawText(vicTitle, (int)(modalRec.x + (modalRec.width - tw) * 0.5f), (int)modalRec.y + 60, FontSize::TITLE_MEDIUM, (Color){ 46, 204, 113, 255 });

    std::string infoText = Localization::IsKorean() ? 
        ("제 " + std::to_string(combat.GetCurrentWave()) + " / " + std::to_string(combat.GetMaxWaves()) + " 웨이브를 정복했습니다!") :
        ("Wave " + std::to_string(combat.GetCurrentWave()) + " of " + std::to_string(combat.GetMaxWaves()) + " Cleared!");
    int itw = FontManager::MeasureText(infoText.c_str(), FontSize::CARD_TITLE + 2);
    FontManager::DrawText(infoText.c_str(), (int)(modalRec.x + (modalRec.width - itw) * 0.5f), (int)modalRec.y + 130, FontSize::CARD_TITLE + 2, WHITE);

    std::string bonusText = Localization::IsKorean() ? "영웅 생명력 +25 회복 & 모든 스킬 쿨다운 초기화" : "Hero Restored +25 Health & Cooldowns Reset";
    int btw = FontManager::MeasureText(bonusText.c_str(), FontSize::BUTTON_LARGE);
    FontManager::DrawText(bonusText.c_str(), (int)(modalRec.x + (modalRec.width - btw) * 0.5f), (int)modalRec.y + 200, FontSize::BUTTON_LARGE, (Color){ 241, 196, 15, 255 });

    std::string btnLabel;
    if (combat.GetCurrentWave() >= combat.GetMaxWaves()) {
        btnLabel = Localization::IsKorean() ? "★ 탑 정복 완료! 다시 플레이 [SPACE / ENTER] ★" : "* ASCEND & PLAY AGAIN [SPACE / ENTER] *";
    } else {
        btnLabel = Localization::IsKorean() ? "다음 웨이브로 진격 [SPACE / ENTER]" : "ADVANCE TO NEXT WAVE [SPACE / ENTER]";
    }
    Rectangle nextRec = { modalRec.x + modalRec.width * 0.5f - 340.0f, modalRec.y + 480.0f, 680.0f, 90.0f };
    CombatRenderer::DrawButton(nextRec, btnLabel.c_str(), (Color){ 39, 174, 96, 255 }, (Color){ 46, 204, 113, 255 }, false, false, FontSize::BUTTON_LARGE);
}

void UIRenderer::DrawGameOverOverlay(const CombatSystem& combat) const {
    (void)combat;
    DrawRectangle(0, 0, virtualWidth, virtualHeight, (Color){ 25, 10, 10, 210 });

    Rectangle modalRec = { (float)virtualWidth * 0.5f - 550.0f, (float)virtualHeight * 0.5f - 350.0f, 1100.0f, 700.0f };
    CombatRenderer::DrawCard(modalRec, (Color){ 35, 20, 20, 250 }, (Color){ 231, 76, 60, 255 }, 0.08f);

    const char* defTitle = Localization::IsKorean() ? "☠ 패배 - 원정 종료 ☠" : "[X] DEFEAT - RUN CONCLUDED [X]";
    int tw = FontManager::MeasureText(defTitle, FontSize::TITLE_MEDIUM);
    FontManager::DrawText(defTitle, (int)(modalRec.x + (modalRec.width - tw) * 0.5f), (int)modalRec.y + 60, FontSize::TITLE_MEDIUM, (Color){ 231, 76, 60, 255 });

    const char* sub = Localization::IsKorean() ? "원소의 폭주와 적들의 공세에 영웅이 쓰러졌습니다." : "The elemental convergence overwhelmed your defenses.";
    int stw = FontManager::MeasureText(sub, FontSize::BUTTON_LARGE);
    FontManager::DrawText(sub, (int)(modalRec.x + (modalRec.width - stw) * 0.5f), (int)modalRec.y + 130, FontSize::BUTTON_LARGE, (Color){ 200, 200, 210, 255 });

    const char* tip = Localization::IsKorean() ? "전략 팁: [방어 태세] (+18 방어막)와 [패링 태세]의 반사 카운터를 적극 활용하세요!" : "Tip: Utilize [Defense Stance] (+18 Shield) and [Parry] to counter devastating bursts!";
    int ttw = FontManager::MeasureText(tip, FontSize::BUTTON_MEDIUM + 2);
    FontManager::DrawText(tip, (int)(modalRec.x + (modalRec.width - ttw) * 0.5f), (int)modalRec.y + 220, FontSize::BUTTON_MEDIUM + 2, (Color){ 241, 196, 15, 255 });

    Rectangle retryRec = { modalRec.x + modalRec.width * 0.5f - 260.0f, modalRec.y + 480.0f, 520.0f, 90.0f };
    const char* retryLabel = Localization::IsKorean() ? "다시 도전하기 [SPACE / ENTER]" : "RETRY RUN [SPACE / ENTER]";
    CombatRenderer::DrawButton(retryRec, retryLabel, (Color){ 192, 57, 43, 255 }, (Color){ 231, 76, 60, 255 }, false, false, FontSize::BUTTON_LARGE);
}

void UIRenderer::DrawSettingsOverlay(int selectedIdx) const {
    DrawRectangle(0, 0, virtualWidth, virtualHeight, (Color){ 10, 12, 18, 235 });

    Rectangle modalRec = { (float)virtualWidth * 0.5f - 750.0f, (float)virtualHeight * 0.5f - 500.0f, 1500.0f, 1000.0f };
    CombatRenderer::DrawCard(modalRec, (Color){ 24, 30, 45, 255 }, (Color){ 241, 196, 15, 255 }, 0.06f);

    std::string title = Localization::IsKorean() ? "비디오 디스플레이 & 언어 설정 옵션" : "VIDEO & LANGUAGE SETTINGS OPTIONS";
    FontManager::DrawText(title.c_str(), (int)modalRec.x + 50, (int)modalRec.y + 45, FontSize::MODAL_HEADER, (Color){ 241, 196, 15, 255 });

    std::string navGuide = Localization::IsKorean() ? 
        "방향키 [W/S]로 항목 이동, [A/D]로 값 변경, 또는 마우스로 버튼을 직접 클릭하세요." :
        "Navigate with [W/S] or [Up/Down], change with [A/D] or [Left/Right], or click directly.";
    FontManager::DrawText(navGuide.c_str(), (int)modalRec.x + 50, (int)modalRec.y + 90, FontSize::BODY_REGULAR, (Color){ 180, 190, 210, 255 });

    // 1. Resolution Options Section
    float y = modalRec.y + 140.0f;
    bool row0Selected = (selectedIdx == 0);
    std::string resHeader = Localization::IsKorean() ? "1. 화면 해상도 설정 (최대 QHD 2560x1440):" : "1. OUTPUT RESOLUTION (UP TO QHD 2560x1440):";
    FontManager::DrawText(resHeader.c_str(), (int)modalRec.x + 50, (int)y, FontSize::BUTTON_LARGE, row0Selected ? (Color){ 241, 196, 15, 255 } : WHITE);
    y += 42.0f;

    const auto& resList = DisplaySettings::GetResolutions();
    int currentResIdx = DisplaySettings::GetCurrentResolutionIndex();

    for (size_t i = 0; i < resList.size(); ++i) {
        Rectangle btnRec = { modalRec.x + 60.0f + (float)i * 340.0f, y, 320.0f, 95.0f };
        bool isActive = (static_cast<int>(i) == currentResIdx);

        std::string btnText = std::string(resList[i].label) + "\n\n" + resList[i].tag;
        CombatRenderer::DrawButton(btnRec, btnText.c_str(), (Color){ 30, 38, 55, 255 }, (Color){ 41, 128, 185, 255 }, isActive, false, FontSize::BUTTON_MEDIUM);
    }

    y += 135.0f;

    // 2. Display Mode & Language Section
    bool row1Selected = (selectedIdx == 1);
    std::string modeHeader = Localization::IsKorean() ? "2. 화면 모드 및 언어(Language) 설정:" : "2. DISPLAY MODE & LANGUAGE:";
    FontManager::DrawText(modeHeader.c_str(), (int)modalRec.x + 50, (int)y, FontSize::BUTTON_LARGE, row1Selected ? (Color){ 241, 196, 15, 255 } : WHITE);
    y += 42.0f;

    bool isFs = IsWindowFullscreen();
    Rectangle winBtnRec = { modalRec.x + 60.0f, y, 320.0f, 85.0f };
    Rectangle fsBtnRec = { modalRec.x + 400.0f, y, 320.0f, 85.0f };
    Rectangle langBtnRec = { modalRec.x + 740.0f, y, 640.0f, 85.0f };

    CombatRenderer::DrawButton(winBtnRec, Localization::IsKorean() ? "창모드 (Windowed)" : "Windowed Mode", (Color){ 30, 38, 55, 255 }, (Color){ 52, 152, 219, 255 }, !isFs, false, FontSize::BUTTON_MEDIUM);
    CombatRenderer::DrawButton(fsBtnRec, Localization::IsKorean() ? "전체화면 [F11]" : "Fullscreen [F11]", (Color){ 30, 38, 55, 255 }, (Color){ 108, 92, 231, 255 }, isFs, false, FontSize::BUTTON_MEDIUM);
    
    std::string langBtnLabel = Localization::IsKorean() ? "현재 언어: [ 한국어 (기본) ] -> [L] 키로 English 전환" : "Current Language: [ English ] -> Press [L] for Korean";
    CombatRenderer::DrawButton(langBtnRec, langBtnLabel.c_str(), (Color){ 30, 38, 55, 255 }, (Color){ 230, 126, 34, 255 }, true, false, FontSize::BUTTON_MEDIUM);

    y += 130.0f;

    // 3. Hardware Specs
    Rectangle infoRec = { modalRec.x + 50.0f, y, modalRec.width - 100.0f, 240.0f };
    DrawRectangleRounded(infoRec, 0.08f, 6, (Color){ 18, 22, 34, 230 });
    DrawRectangleRoundedLinesEx(infoRec, 0.08f, 6, 2.0f, (Color){ 65, 75, 95, 255 });

    int monitor = GetCurrentMonitor();
    int monW = GetMonitorWidth(monitor);
    int monH = GetMonitorHeight(monitor);

    FontManager::DrawText(Localization::IsKorean() ? "그래픽 및 하드웨어 가속 사양:" : "GRAPHICS & HARDWARE SPECIFICATIONS:", (int)infoRec.x + 25, (int)infoRec.y + 18, FontSize::BODY_REGULAR, (Color){ 241, 196, 15, 255 });
    
    std::string internalRes = Localization::IsKorean() ? 
        ("- 내부 렌더 타겟 캔버스: 2560 x 1440 (Native QHD 고해상도 벡터 버퍼)") :
        ("- Internal Render Target Canvas: 2560 x 1440 (Native QHD High-DPI Vector Buffer)");
    std::string scalingMode = Localization::IsKorean() ?
        ("- 화면 스케일링 파이프라인: 하드웨어 바이리니어 필터링 + 16:9 레터박스 자동 보정") :
        ("- Scaling Pipeline: Hardware Bilinear Filtering + 16:9 Letterboxing Filter");
    std::string monInfo = Localization::IsKorean() ?
        ("- 모니터 기본 해상도: " + std::to_string(monW) + " x " + std::to_string(monH)) :
        ("- Primary Monitor Native Resolution: " + std::to_string(monW) + " x " + std::to_string(monH));
    std::string currentWin = Localization::IsKorean() ?
        ("- 현재 윈도우 출력 크기: " + std::to_string(GetScreenWidth()) + " x " + std::to_string(GetScreenHeight()) + (isFs ? " (전체화면)" : " (창모드)")) :
        ("- Current Window Output: " + std::to_string(GetScreenWidth()) + " x " + std::to_string(GetScreenHeight()) + (isFs ? " (Fullscreen)" : " (Windowed)"));

    FontManager::DrawText(internalRes.c_str(), (int)infoRec.x + 25, (int)infoRec.y + 55, FontSize::BODY_SMALL, (Color){ 190, 200, 220, 255 });
    FontManager::DrawText(scalingMode.c_str(), (int)infoRec.x + 25, (int)infoRec.y + 90, FontSize::BODY_SMALL, (Color){ 190, 200, 220, 255 });
    FontManager::DrawText(monInfo.c_str(), (int)infoRec.x + 25, (int)infoRec.y + 125, FontSize::BODY_SMALL, (Color){ 190, 200, 220, 255 });
    FontManager::DrawText(currentWin.c_str(), (int)infoRec.x + 25, (int)infoRec.y + 160, FontSize::BODY_SMALL, (Color){ 46, 204, 113, 255 });

    // Close Button
    bool row2Selected = (selectedIdx >= 2);
    Rectangle closeRec = { modalRec.x + modalRec.width * 0.5f - 180.0f, modalRec.y + modalRec.height - 90.0f, 360.0f, 65.0f };
    const char* closeLabel = Localization::IsKorean() ? "설정 적용 및 닫기 [ESC / O]" : "Apply & Close [ESC / O]";
    CombatRenderer::DrawButton(closeRec, closeLabel, (Color){ 39, 174, 96, 255 }, (Color){ 46, 204, 113, 255 }, row2Selected, false, FontSize::BUTTON_MEDIUM + 2);
}

void UIRenderer::DrawGuideOverlay() const {
    DrawRectangle(0, 0, virtualWidth, virtualHeight, (Color){ 10, 12, 18, 230 });

    Rectangle modalRec = { (float)virtualWidth * 0.5f - 700.0f, (float)virtualHeight * 0.5f - 480.0f, 1400.0f, 960.0f };
    CombatRenderer::DrawCard(modalRec, (Color){ 24, 30, 45, 255 }, (Color){ 241, 196, 15, 255 }, 0.06f);

    const char* guideTitle = Localization::IsKorean() ? "원소 반응 매트릭스 & 날씨 시스템 도감" : "ELEMENTAL REACTION & WEATHER COMPENDIUM";
    FontManager::DrawText(guideTitle, (int)modalRec.x + 50, (int)modalRec.y + 40, FontSize::MODAL_HEADER - 2, (Color){ 241, 196, 15, 255 });

    float y = modalRec.y + 100.0f;
    FontManager::DrawText(Localization::IsKorean() ? "핵심 원소 반응 조합:" : "CORE ELEMENTAL REACTIONS:", (int)modalRec.x + 50, (int)y, FontSize::BUTTON_LARGE, WHITE);
    y += 38.0f;

    if (Localization::IsKorean()) {
        FontManager::DrawText("- [수분] + [전기] = [감전 (SHOCK)]     -> 강력한 추가 피해 + 모든 적에게 12 광역 전도 피해!", (int)modalRec.x + 60, (int)y, FontSize::BODY_SMALL + 1, (Color){ 241, 196, 15, 255 });
        y += 34.0f;
        FontManager::DrawText("- [기름] + [화염] = [폭발 (EXPLOSION)] -> 대규모 폭발 피해 + 3턴 지속 화상(DoT) 부여!", (int)modalRec.x + 60, (int)y, FontSize::BODY_SMALL + 1, (Color){ 231, 76, 60, 255 });
        y += 34.0f;
        FontManager::DrawText("- [수분] + [냉기] = [빙결 (FROZEN)]    -> 대상을 단단히 얼려 다음 턴 행동 완전 무효화!", (int)modalRec.x + 60, (int)y, FontSize::BODY_SMALL + 1, (Color){ 162, 222, 255, 255 });
        y += 34.0f;
        FontManager::DrawText("- [화염] + [냉기] = [증발/융해 (MELT)]  -> 초고온 증기 기화 반응으로 강력한 관통 피해.", (int)modalRec.x + 60, (int)y, FontSize::BODY_SMALL + 1, (Color){ 243, 156, 18, 255 });
        y += 34.0f;
        FontManager::DrawText("- [기름] + [전기] = [플라즈마 (PLASMA)] -> 방어막을 무시하고 직접 관통하는 전도 피해.", (int)modalRec.x + 60, (int)y, FontSize::BODY_SMALL + 1, (Color){ 155, 89, 182, 255 });
        y += 48.0f;

        FontManager::DrawText("동적 날씨 예보 효과 & 전장 환경:", (int)modalRec.x + 50, (int)y, FontSize::BUTTON_LARGE, WHITE);
        y += 38.0f;
        FontManager::DrawText("- 폭우 (Rain): 매 턴 전장에 [수분] 부여 | 물 속성 스킬 피해 +35% 증폭", (int)modalRec.x + 60, (int)y, FontSize::BODY_SMALL + 1, (Color){ 52, 152, 219, 255 });
        y += 34.0f;
        FontManager::DrawText("- 폭염 (Heatwave): 극심한 건조 열파 | 화염 스킬 피해 +50% 파격 증폭", (int)modalRec.x + 60, (int)y, FontSize::BODY_SMALL + 1, (Color){ 230, 126, 34, 255 });
        y += 34.0f;
        FontManager::DrawText("- 뇌우 (Storm): [수분] 부여 + 무작위 적에게 15 위력의 낙뢰 번개 강타!", (int)modalRec.x + 60, (int)y, FontSize::BODY_SMALL + 1, (Color){ 142, 68, 173, 255 });
        y += 34.0f;
        FontManager::DrawText("- 눈보라 (Blizzard): [수분]에 젖은 적 즉시 빙결 | 냉기 스킬 피해 +30%", (int)modalRec.x + 60, (int)y, FontSize::BODY_SMALL + 1, (Color){ 129, 236, 236, 255 });
        y += 34.0f;
        FontManager::DrawText("- 돌풍 (Gale): 소용돌이 바람이 적들의 모든 상태이상을 전장 전체로 확산 전파!", (int)modalRec.x + 60, (int)y, FontSize::BODY_SMALL + 1, (Color){ 46, 204, 113, 255 });
        y += 34.0f;
        FontManager::DrawText("- 산성비 (Acid Rain): 모든 대상을 인화성 [기름]으로 코팅!", (int)modalRec.x + 60, (int)y, FontSize::BODY_SMALL + 1, (Color){ 108, 92, 231, 255 });
    } else {
        FontManager::DrawText("- [WET] + [LIGHTNING] = [SHOCK]    -> Massive burst + Arcs AoE chain damage to all alive enemies!", (int)modalRec.x + 60, (int)y, FontSize::BODY_SMALL + 1, (Color){ 241, 196, 15, 255 });
        y += 34.0f;
        FontManager::DrawText("- [OIL] + [FIRE] = [EXPLOSION]     -> Cataclysmic burst damage + inflicts Burning DoT to target!", (int)modalRec.x + 60, (int)y, FontSize::BODY_SMALL + 1, (Color){ 231, 76, 60, 255 });
        y += 34.0f;
        FontManager::DrawText("- [WET] + [COLD] = [FROZEN]        -> Immobilizes target completely; skips their next turn!", (int)modalRec.x + 60, (int)y, FontSize::BODY_SMALL + 1, (Color){ 162, 222, 255, 255 });
        y += 34.0f;
        FontManager::DrawText("- [FIRE] + [COLD] = [MELT]         -> Superheated steam vaporization bonus damage.", (int)modalRec.x + 60, (int)y, FontSize::BODY_SMALL + 1, (Color){ 243, 156, 18, 255 });
        y += 34.0f;
        FontManager::DrawText("- [OIL] + [LIGHTNING] = [PLASMA]   -> Piercing discharge bonus damage bypassing armor.", (int)modalRec.x + 60, (int)y, FontSize::BODY_SMALL + 1, (Color){ 155, 89, 182, 255 });
        y += 48.0f;

        FontManager::DrawText("DYNAMIC WEATHER FORECAST MECHANICS:", (int)modalRec.x + 50, (int)y, FontSize::BUTTON_LARGE, WHITE);
        y += 38.0f;
        FontManager::DrawText("- Rain: Applies [WET] globally each turn | Boosts Water DMG +35%", (int)modalRec.x + 60, (int)y, FontSize::BODY_SMALL + 1, (Color){ 52, 152, 219, 255 });
        y += 34.0f;
        FontManager::DrawText("- Heatwave: Extreme dry heat | Boosts Fire DMG +50%", (int)modalRec.x + 60, (int)y, FontSize::BODY_SMALL + 1, (Color){ 230, 126, 34, 255 });
        y += 34.0f;
        FontManager::DrawText("- Storm: Applies [WET] globally + Calls down 15 DMG Lightning Strikes!", (int)modalRec.x + 60, (int)y, FontSize::BODY_SMALL + 1, (Color){ 142, 68, 173, 255 });
        y += 34.0f;
        FontManager::DrawText("- Blizzard: Freezes all [WET] targets instantly | Boosts Cold DMG +30%", (int)modalRec.x + 60, (int)y, FontSize::BODY_SMALL + 1, (Color){ 129, 236, 236, 255 });
        y += 34.0f;
        FontManager::DrawText("- Gale Winds: Swirls & spreads active elemental debuffs across all enemies!", (int)modalRec.x + 60, (int)y, FontSize::BODY_SMALL + 1, (Color){ 46, 204, 113, 255 });
        y += 34.0f;
        FontManager::DrawText("- Acid Rain: Coats all units in combustible [OIL]!", (int)modalRec.x + 60, (int)y, FontSize::BODY_SMALL + 1, (Color){ 108, 92, 231, 255 });
    }

    Rectangle closeRec = { modalRec.x + modalRec.width * 0.5f - 140.0f, modalRec.y + modalRec.height - 90.0f, 280.0f, 60.0f };
    const char* closeGuide = Localization::IsKorean() ? "도감 닫기 [ESC]" : "Close Guide [ESC]";
    CombatRenderer::DrawButton(closeRec, closeGuide, (Color){ 39, 174, 96, 255 }, (Color){ 46, 204, 113, 255 }, false, false, FontSize::BUTTON_MEDIUM + 2);
}
