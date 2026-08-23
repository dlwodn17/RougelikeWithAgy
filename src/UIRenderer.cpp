#include "UIRenderer.hpp"
#include <iomanip>
#include <sstream>

UIRenderer::UIRenderer() : fontLoaded(false), showCheatSheet(false) {
}

UIRenderer::~UIRenderer() {
}

void UIRenderer::Initialize() {
    fontLoaded = false;
}

void UIRenderer::DrawCustomCard(Rectangle rec, Color bg, Color border, float roundness) {
    DrawRectangleRounded(rec, roundness, 8, bg);
    DrawRectangleRoundedLinesEx(rec, roundness, 8, 3.0f, border);
}

bool UIRenderer::DrawButton(Rectangle rec, const char* text, Color baseColor, Color hoverColor, bool active, bool disabled) {
    Vector2 mousePos = ScreenConfig::GetVirtualMousePosition();
    bool hovered = CheckCollisionPointRec(mousePos, rec) && !disabled;
    bool clicked = hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    Color bg = disabled ? (Color){ 50, 54, 65, 220 } : (active ? hoverColor : (hovered ? hoverColor : baseColor));
    Color border = active ? (Color){ 241, 196, 15, 255 } : (hovered ? WHITE : (Color){ 100, 115, 140, 255 });

    DrawCustomCard(rec, bg, border, 0.12f);

    int fontSize = 24;
    int textWidth = MeasureText(text, fontSize);
    int textX = (int)(rec.x + (rec.width - textWidth) * 0.5f);
    int textY = (int)(rec.y + (rec.height - fontSize) * 0.5f);

    Color textColor = disabled ? (Color){ 140, 145, 155, 255 } : WHITE;
    DrawText(text, textX, textY, fontSize, textColor);

    return clicked;
}

void UIRenderer::DrawBackground(WeatherType weather) {
    int w = ScreenConfig::VIRTUAL_WIDTH;
    int h = ScreenConfig::VIRTUAL_HEIGHT;

    // Dark moody gradient base
    Color topCol = (Color){ 15, 20, 30, 255 };
    Color botCol = (Color){ 25, 30, 45, 255 };

    if (weather == WeatherType::HEATWAVE) {
        topCol = (Color){ 40, 20, 18, 255 };
        botCol = (Color){ 50, 28, 22, 255 };
    } else if (weather == WeatherType::BLIZZARD) {
        topCol = (Color){ 14, 28, 45, 255 };
        botCol = (Color){ 22, 38, 60, 255 };
    } else if (weather == WeatherType::THUNDERSTORM) {
        topCol = (Color){ 22, 16, 38, 255 };
        botCol = (Color){ 30, 22, 50, 255 };
    } else if (weather == WeatherType::RAIN) {
        topCol = (Color){ 16, 26, 38, 255 };
        botCol = (Color){ 22, 36, 52, 255 };
    } else if (weather == WeatherType::ACID_RAIN) {
        topCol = (Color){ 28, 16, 38, 255 };
        botCol = (Color){ 38, 22, 52, 255 };
    }

    DrawRectangleGradientV(0, 0, w, h, topCol, botCol);

    // Subtle arena floor grid
    Color gridColor = (Color){ 255, 255, 255, 10 };
    for (int y = 780; y < h; y += 50) {
        DrawLine(0, y, w, y, gridColor);
    }
}

void UIRenderer::DrawWeatherForecast(const WeatherSystem& weatherSystem) {
    int w = ScreenConfig::VIRTUAL_WIDTH;
    Rectangle barRec = { 40, 20, (float)w - 80, 110 };
    DrawCustomCard(barRec, (Color){ 20, 25, 38, 230 }, (Color){ 70, 80, 105, 255 }, 0.08f);

    // Weather label
    DrawText("WEATHER FORECAST", 65, 38, 20, (Color){ 160, 175, 200, 255 });
    DrawText("[1-3 Turns Ahead Queue]", 65, 68, 15, (Color){ 120, 130, 150, 255 });

    // Active Weather Display (Slot 0 - Current Turn)
    WeatherType current = weatherSystem.GetCurrentWeather();
    Rectangle slot0Rec = { 380, 30, 480, 90 };
    Color curColor = WeatherSystem::GetWeatherColor(current);
    DrawCustomCard(slot0Rec, (Color){ 30, 38, 55, 255 }, curColor, 0.12f);

    std::string activeTitle = "ACTIVE: " + std::string(WeatherSystem::GetWeatherIcon(current));
    DrawText(activeTitle.c_str(), 400, 40, 26, curColor);
    DrawText(WeatherSystem::GetWeatherShortDesc(current), 400, 75, 18, (Color){ 210, 220, 235, 255 });

    // Upcoming forecast slots (1, 2, 3)
    const auto& queue = weatherSystem.GetForecastQueue();
    float startX = 890;
    for (size_t i = 0; i < queue.size() && i < 3; ++i) {
        WeatherType nextW = queue[i];
        Rectangle nextRec = { startX + (float)i * 350, 30, 330, 90 };
        DrawCustomCard(nextRec, (Color){ 25, 30, 42, 200 }, (Color){ 60, 70, 90, 200 }, 0.10f);

        std::string turnLabel = "+" + std::to_string(i + 1) + " Turn: " + WeatherSystem::GetWeatherName(nextW);
        DrawText(turnLabel.c_str(), (int)nextRec.x + 18, 42, 20, WeatherSystem::GetWeatherColor(nextW));
        DrawText(WeatherSystem::GetWeatherShortDesc(nextW), (int)nextRec.x + 18, 74, 15, (Color){ 160, 170, 190, 255 });
    }

    // Reaction Guide Button
    Rectangle helpRec = { (float)w - 560, 35, 240, 80 };
    bool clickedHelp = DrawButton(helpRec, showCheatSheet ? "Guide [H]" : "Guide [H]", (Color){ 41, 128, 185, 255 }, (Color){ 52, 152, 219, 255 });
    if (clickedHelp || IsKeyPressed(KEY_H) || IsKeyPressed(KEY_TAB)) {
        showCheatSheet = !showCheatSheet;
    }

    // Fullscreen Toggle Button (F11)
    Rectangle fsRec = { (float)w - 290, 35, 220, 80 };
    bool isFs = IsWindowFullscreen();
    std::string fsText = isFs ? "Window [F11]" : "Full [F11]";
    if (DrawButton(fsRec, fsText.c_str(), (Color){ 108, 92, 231, 255 }, (Color){ 155, 89, 182, 255 }) || IsKeyPressed(KEY_F11)) {
        ScreenConfig::ToggleGameFullscreen();
    }
}

void UIRenderer::DrawHealthBar(Vector2 pos, Vector2 size, int currentHp, int maxHp, int shield, Color fillColor) {
    // Background
    DrawRectangleRounded((Rectangle){ pos.x, pos.y, size.x, size.y }, 0.25f, 6, (Color){ 30, 35, 45, 255 });

    // HP Fill
    float hpPercent = (maxHp > 0) ? std::max(0.0f, std::min(1.0f, (float)currentHp / (float)maxHp)) : 0.0f;
    if (hpPercent > 0.0f) {
        DrawRectangleRounded((Rectangle){ pos.x, pos.y, size.x * hpPercent, size.y }, 0.25f, 6, fillColor);
    }

    // Shield Overlay (if any)
    if (shield > 0) {
        float shieldPercent = (maxHp > 0) ? std::min(1.0f, (float)shield / (float)maxHp) : 0.0f;
        DrawRectangleRounded((Rectangle){ pos.x, pos.y, size.x * shieldPercent, size.y * 0.4f }, 0.2f, 6, (Color){ 100, 200, 255, 220 });
    }

    // Outline
    DrawRectangleRoundedLinesEx((Rectangle){ pos.x, pos.y, size.x, size.y }, 0.25f, 6, 2.0f, (Color){ 80, 90, 110, 255 });

    // Text (HP / MaxHP + Shield)
    std::string hpText = std::to_string(currentHp) + " / " + std::to_string(maxHp);
    if (shield > 0) {
        hpText += " (+" + std::to_string(shield) + " SHIELD)";
    }
    int fontSize = 20;
    int tw = MeasureText(hpText.c_str(), fontSize);
    DrawText(hpText.c_str(), (int)(pos.x + (size.x - tw) * 0.5f), (int)(pos.y + (size.y - fontSize) * 0.5f), fontSize, WHITE);
}

void UIRenderer::DrawStatusBadges(const std::vector<StatusInstance>& statuses, Vector2 startPos) {
    float xOffset = 0;
    for (const auto& st : statuses) {
        if (st.element == Element::NONE || st.duration <= 0) continue;

        Color col = ElementalSystem::GetElementColor(st.element);
        std::string badgeText = std::string(ElementalSystem::GetElementIcon(st.element)) + " (" + std::to_string(st.duration) + "T)";
        int fontSize = 18;
        int tw = MeasureText(badgeText.c_str(), fontSize);
        Rectangle badgeRec = { startPos.x + xOffset, startPos.y, (float)tw + 20, 36 };

        DrawRectangleRounded(badgeRec, 0.3f, 6, (Color){ 20, 25, 35, 240 });
        DrawRectangleRoundedLinesEx(badgeRec, 0.3f, 6, 2.0f, col);
        DrawText(badgeText.c_str(), (int)badgeRec.x + 10, (int)badgeRec.y + 8, fontSize, col);

        xOffset += badgeRec.width + 12;
    }
}

void UIRenderer::DrawPlayerCard(const Player& player, StanceType selectedStance) {
    Vector2 offset = player.GetRenderOffset();
    Rectangle cardRec = { 60 + offset.x, 160 + offset.y, 620, 620 };

    // Border glows based on active stance
    Color borderColor = (Color){ 52, 152, 219, 255 };
    if (selectedStance == StanceType::ATTACK) borderColor = (Color){ 231, 76, 60, 255 };
    else if (selectedStance == StanceType::DEFENSE) borderColor = (Color){ 46, 204, 113, 255 };
    else if (selectedStance == StanceType::PARRY) borderColor = (Color){ 241, 196, 15, 255 };

    DrawCustomCard(cardRec, (Color){ 24, 30, 45, 240 }, borderColor, 0.06f);

    // Player Title / Avatar silhouette
    DrawRectangle((int)cardRec.x + 30, (int)cardRec.y + 30, 90, 90, (Color){ 41, 128, 185, 220 });
    DrawRectangleLinesEx((Rectangle){ cardRec.x + 30, cardRec.y + 30, 90, 90 }, 2.5f, borderColor);
    DrawText("HERO", (int)cardRec.x + 46, (int)cardRec.y + 64, 20, WHITE);

    DrawText(player.GetName().c_str(), (int)cardRec.x + 140, (int)cardRec.y + 35, 30, WHITE);

    // Stance Badge
    const char* stanceName = "⚔️ ATTACK STANCE (+40% DMG)";
    Color stanceCol = (Color){ 231, 76, 60, 255 };
    if (selectedStance == StanceType::DEFENSE) {
        stanceName = "🛡️ DEFENSE (+18 Shield, -30% DMG)";
        stanceCol = (Color){ 46, 204, 113, 255 };
    } else if (selectedStance == StanceType::PARRY) {
        stanceName = "⚡ PARRY (Counter & Reflect)";
        stanceCol = (Color){ 241, 196, 15, 255 };
    }
    DrawText(stanceName, (int)cardRec.x + 140, (int)cardRec.y + 78, 20, stanceCol);

    // HP Bar
    DrawText("HEALTH & SHIELD POINTS", (int)cardRec.x + 30, (int)cardRec.y + 145, 18, (Color){ 160, 175, 200, 255 });
    DrawHealthBar((Vector2){ cardRec.x + 30, cardRec.y + 175 }, (Vector2){ 560, 42 }, player.GetHp(), player.GetMaxHp(), player.GetShield(), (Color){ 46, 204, 113, 255 });

    // Active Status Buffers
    DrawText("ACTIVE ELEMENTAL STATUS BUFFER:", (int)cardRec.x + 30, (int)cardRec.y + 245, 18, (Color){ 160, 175, 200, 255 });
    if (player.GetStatusBuffer().empty()) {
        DrawText("(Clean - No active elemental debuffs)", (int)cardRec.x + 30, (int)cardRec.y + 280, 18, (Color){ 120, 130, 150, 255 });
    } else {
        DrawStatusBadges(player.GetStatusBuffer(), (Vector2){ cardRec.x + 30, cardRec.y + 280 });
    }

    // Strategy & Tips Card
    Rectangle traitRec = { cardRec.x + 25, cardRec.y + 350, 570, 235 };
    DrawRectangleRounded(traitRec, 0.08f, 6, (Color){ 18, 22, 34, 220 });
    DrawText("COMBAT TACTICS & HOTKEYS:", (int)traitRec.x + 20, (int)traitRec.y + 18, 20, (Color){ 241, 196, 15, 255 });
    DrawText("• Skills: [1] Torrent [2] Ignition [3] Thunder [4] Glacial", (int)traitRec.x + 20, (int)traitRec.y + 55, 17, (Color){ 190, 200, 220, 255 });
    DrawText("• Stance: [Q] Attack (+40% DMG) [W] Defense [E] Parry", (int)traitRec.x + 20, (int)traitRec.y + 85, 17, (Color){ 190, 200, 220, 255 });
    DrawText("• Execution: Press [SPACE] or [ENTER] to Execute Turn", (int)traitRec.x + 20, (int)traitRec.y + 115, 17, (Color){ 190, 200, 220, 255 });
    DrawText("• Reactions: Combine WET + ELEC (Shock) / OIL + FIRE (Explosion)", (int)traitRec.x + 20, (int)traitRec.y + 145, 17, (Color){ 241, 196, 15, 255 });
    DrawText("• Press [F11] to Toggle Fullscreen anytime", (int)traitRec.x + 20, (int)traitRec.y + 175, 17, (Color){ 108, 92, 231, 255 });
}

void UIRenderer::DrawEnemyCards(CombatSystem& combat) {
    const auto& enemies = combat.GetEnemies();
    int selectedIdx = combat.GetSelectedTargetIndex();
    Vector2 mousePos = ScreenConfig::GetVirtualMousePosition();

    float startX = 720.0f;
    float cardWidth = 560.0f;
    float spacing = 35.0f;

    if (enemies.size() == 2) {
        startX = 900.0f;
        cardWidth = 720.0f;
        spacing = 60.0f;
    } else if (enemies.size() >= 3) {
        startX = 720.0f;
        cardWidth = 570.0f;
        spacing = 30.0f;
    }

    for (size_t i = 0; i < enemies.size(); ++i) {
        const Enemy& enemy = enemies[i];
        if (!enemy.IsAlive()) continue;

        Vector2 offset = enemy.GetRenderOffset();
        Rectangle rec = { startX + (float)i * (cardWidth + spacing) + offset.x, 160.0f + offset.y, cardWidth, 620.0f };

        bool isSelected = (static_cast<int>(i) == selectedIdx);
        bool isHovered = CheckCollisionPointRec(mousePos, rec);

        if (isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            combat.SelectTarget((int)i);
        }

        // Selection / Hover highlights
        Color border = isSelected ? (Color){ 241, 196, 15, 255 } : (isHovered ? WHITE : (Color){ 65, 75, 95, 255 });
        Color bg = isSelected ? (Color){ 35, 30, 50, 245 } : (Color){ 25, 30, 42, 235 };

        DrawCustomCard(rec, bg, border, 0.06f);

        // Target marker arrow above card
        if (isSelected) {
            DrawText("▼ ACTIVE TARGET ▼", (int)(rec.x + rec.width * 0.5f - 110), (int)rec.y - 30, 22, (Color){ 241, 196, 15, 255 });
        }

        // Enemy Name & Type
        DrawText(enemy.GetName().c_str(), (int)rec.x + 25, (int)rec.y + 25, 28, enemy.GetColor());

        // Intent Bubble above Health
        const Intent& intent = enemy.GetIntent();
        Rectangle intentRec = { rec.x + 25, rec.y + 70, rec.width - 50, 75 };
        Color intentBorder = GetElementBaseColor(intent.element);
        DrawCustomCard(intentRec, (Color){ 18, 20, 30, 230 }, intentBorder, 0.10f);

        std::string intentText = "INTENT: " + intent.name;
        if (intent.type == IntentType::ATTACK) {
            intentText += " (" + std::to_string(intent.value) + " ⚔️ DAMAGE)";
        } else if (intent.type == IntentType::DEFEND) {
            intentText += " (+" + std::to_string(intent.value) + " 🛡️ SHIELD)";
        }
        DrawText(intentText.c_str(), (int)intentRec.x + 16, (int)intentRec.y + 12, 20, intentBorder);
        DrawText(intent.desc.c_str(), (int)intentRec.x + 16, (int)intentRec.y + 42, 16, (Color){ 180, 190, 205, 255 });

        // Health Bar
        DrawText("HP & SHIELD", (int)rec.x + 25, (int)rec.y + 160, 18, (Color){ 160, 175, 200, 255 });
        DrawHealthBar((Vector2){ rec.x + 25, rec.y + 190 }, (Vector2){ rec.width - 50, 42 }, enemy.GetHp(), enemy.GetMaxHp(), enemy.GetShield(), (Color){ 231, 76, 60, 255 });

        // Status Buffer
        DrawText("STATUS EFFECT BUFFER:", (int)rec.x + 25, (int)rec.y + 255, 18, (Color){ 160, 175, 200, 255 });
        if (enemy.GetStatusBuffer().empty()) {
            DrawText("(No active elemental status)", (int)rec.x + 25, (int)rec.y + 290, 18, (Color){ 120, 130, 150, 255 });
        } else {
            DrawStatusBadges(enemy.GetStatusBuffer(), (Vector2){ rec.x + 25, rec.y + 290 });
        }

        // Frozen / Stun Indicator
        if (enemy.IsFrozen()) {
            Rectangle freezeRec = { rec.x + 25, rec.y + 360, rec.width - 50, 55 };
            DrawRectangleRounded(freezeRec, 0.15f, 6, (Color){ 41, 128, 185, 220 });
            DrawText("🧊 FROZEN (Next Action Skipped)", (int)freezeRec.x + 30, (int)freezeRec.y + 15, 22, WHITE);
        }

        // Click to Target Hint
        if (!isSelected) {
            DrawText("[ Click to Target Enemy ]", (int)(rec.x + rec.width * 0.5f - 110), (int)rec.y + 560, 18, (Color){ 130, 140, 160, 255 });
        }
    }
}

void UIRenderer::DrawStanceSelector(CombatSystem& combat) {
    StanceType currentStance = combat.GetSelectedStance();
    bool isInputPhase = (combat.GetPhase() == CombatPhase::PLAYER_INPUT);

    Rectangle panelRec = { 60, 810, 620, 260 };
    DrawCustomCard(panelRec, (Color){ 22, 28, 42, 230 }, (Color){ 65, 75, 95, 255 }, 0.08f);

    DrawText("STANCE SELECTION [ Q / W / E ]", (int)panelRec.x + 25, (int)panelRec.y + 20, 22, (Color){ 241, 196, 15, 255 });

    // 3 Stance Buttons
    Rectangle atkRec = { panelRec.x + 20, panelRec.y + 65, 180, 170 };
    Rectangle defRec = { panelRec.x + 220, panelRec.y + 65, 180, 170 };
    Rectangle parRec = { panelRec.x + 420, panelRec.y + 65, 180, 170 };

    bool atkActive = (currentStance == StanceType::ATTACK);
    bool defActive = (currentStance == StanceType::DEFENSE);
    bool parActive = (currentStance == StanceType::PARRY);

    if (DrawButton(atkRec, "⚔️ ATK [Q]\n\n+40% DMG", (Color){ 192, 57, 43, 220 }, (Color){ 231, 76, 60, 255 }, atkActive, !isInputPhase) || (isInputPhase && IsKeyPressed(KEY_Q))) {
        combat.SelectStance(StanceType::ATTACK);
    }

    if (DrawButton(defRec, "🛡️ DEF [W]\n\n+18 Shield\n-30% DMG", (Color){ 39, 174, 96, 220 }, (Color){ 46, 204, 113, 255 }, defActive, !isInputPhase) || (isInputPhase && IsKeyPressed(KEY_W))) {
        combat.SelectStance(StanceType::DEFENSE);
    }

    if (DrawButton(parRec, "⚡ PAR [E]\n\nReflect\nCounter", (Color){ 211, 84, 0, 220 }, (Color){ 243, 156, 18, 255 }, parActive, !isInputPhase) || (isInputPhase && IsKeyPressed(KEY_E))) {
        combat.SelectStance(StanceType::PARRY);
    }
}

void UIRenderer::DrawSkillCards(CombatSystem& combat) {
    auto& skills = combat.GetSkillSystem().GetSkills();
    int selectedSkill = combat.GetSelectedSkillIndex();
    bool isInputPhase = (combat.GetPhase() == CombatPhase::PLAYER_INPUT);

    float startX = 720.0f;
    float cardWidth = 330.0f;
    float cardHeight = 260.0f;
    float spacing = 25.0f;

    for (size_t i = 0; i < skills.size(); ++i) {
        const Skill& skill = skills[i];
        Rectangle rec = { startX + (float)i * (cardWidth + spacing), 810.0f, cardWidth, cardHeight };

        bool isSelected = (static_cast<int>(i) == selectedSkill);
        bool isReady = skill.IsReady();
        Vector2 mousePos = ScreenConfig::GetVirtualMousePosition();
        bool hovered = CheckCollisionPointRec(mousePos, rec) && isInputPhase;

        if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            combat.SelectSkill((int)i);
        }

        // Hotkeys 1, 2, 3, 4
        if (isInputPhase && IsKeyPressed(KEY_ONE + (int)i)) {
            combat.SelectSkill((int)i);
        }

        Color border = isSelected ? (Color){ 241, 196, 15, 255 } : (hovered ? WHITE : (Color){ 65, 75, 95, 255 });
        Color bg = isReady ? (isSelected ? (Color){ 35, 42, 60, 255 } : (Color){ 25, 30, 44, 235 }) : (Color){ 20, 22, 30, 210 };

        DrawCustomCard(rec, bg, border, 0.08f);

        // Header: Hotkey + Skill Name
        std::string hotkeyTitle = "[" + std::to_string(i + 1) + "] " + skill.name;
        DrawText(hotkeyTitle.c_str(), (int)rec.x + 18, (int)rec.y + 18, 22, isReady ? skill.themeColor : (Color){ 130, 135, 145, 255 });

        // Element tag badge
        const char* elemName = ElementalSystem::GetElementName(skill.primaryElement);
        DrawText(elemName, (int)rec.x + 18, (int)rec.y + 55, 18, GetElementBaseColor(skill.primaryElement));

        // Damage display
        std::string dmgText = std::to_string(skill.baseDamage) + " DMG";
        DrawText(dmgText.c_str(), (int)rec.x + (int)rec.width - 110, (int)rec.y + 55, 20, (Color){ 241, 196, 15, 255 });

        // Description
        DrawText(skill.description.c_str(), (int)rec.x + 18, (int)rec.y + 95, 15, (Color){ 180, 190, 205, 255 });

        // Cooldown overlay if on cooldown
        if (!isReady) {
            DrawRectangleRounded(rec, 0.08f, 6, (Color){ 10, 12, 18, 220 });
            std::string cdText = "COOLDOWN: " + std::to_string(skill.currentCooldown) + "T";
            int tw = MeasureText(cdText.c_str(), 26);
            DrawText(cdText.c_str(), (int)(rec.x + (rec.width - tw) * 0.5f), (int)(rec.y + 115), 26, (Color){ 231, 76, 60, 255 });
        }
    }
}

void UIRenderer::DrawExecuteButton(CombatSystem& combat) {
    bool isInputPhase = (combat.GetPhase() == CombatPhase::PLAYER_INPUT);
    Rectangle rec = { 2160, 810, 330, 260 };

    bool clicked = DrawButton(rec, "⚡ EXECUTE\n   TURN\n\n  [SPACE]", (Color){ 39, 174, 96, 220 }, (Color){ 46, 204, 113, 255 }, false, !isInputPhase);

    if ((clicked || (isInputPhase && IsKeyPressed(KEY_SPACE))) && isInputPhase) {
        combat.ExecutePlayerTurn();
    }
}

void UIRenderer::DrawCombatLog(const std::vector<CombatLogEntry>& log) {
    int w = ScreenConfig::VIRTUAL_WIDTH;
    Rectangle logRec = { 60, 1100, (float)w - 120, 290 };
    DrawCustomCard(logRec, (Color){ 18, 22, 32, 245 }, (Color){ 55, 65, 85, 255 }, 0.06f);

    DrawText("COMBAT ACTION LOG", (int)logRec.x + 25, (int)logRec.y + 15, 20, (Color){ 160, 175, 200, 255 });

    // Show last 8 log entries
    int maxEntries = 8;
    int startIndex = std::max(0, (int)log.size() - maxEntries);
    int lineY = (int)logRec.y + 48;

    for (size_t i = startIndex; i < log.size(); ++i) {
        DrawText(log[i].text.c_str(), (int)logRec.x + 25, lineY, 20, log[i].color);
        lineY += 28;
    }
}

void UIRenderer::DrawSynergyGuideModal() {
    int w = ScreenConfig::VIRTUAL_WIDTH;
    int h = ScreenConfig::VIRTUAL_HEIGHT;

    // Dark backdrop
    DrawRectangle(0, 0, w, h, (Color){ 10, 12, 18, 230 });

    Rectangle modalRec = { (float)w * 0.5f - 700, (float)h * 0.5f - 480, 1400, 960 };
    DrawCustomCard(modalRec, (Color){ 24, 30, 45, 255 }, (Color){ 241, 196, 15, 255 }, 0.06f);

    DrawText("★ ELEMENTAL REACTION & WEATHER COMPENDIUM", (int)modalRec.x + 50, (int)modalRec.y + 45, 34, (Color){ 241, 196, 15, 255 });

    // Reactions Table
    float y = modalRec.y + 110;
    DrawText("CORE ELEMENTAL REACTIONS:", (int)modalRec.x + 50, (int)y, 24, WHITE);
    y += 40;

    DrawText("• 💧 WET + ⚡ LIGHTNING = [SHOCK]    -> Massive burst + Arcs AoE chain damage to all alive enemies!", (int)modalRec.x + 60, (int)y, 20, (Color){ 241, 196, 15, 255 });
    y += 36;
    DrawText("• 🛢️ OIL + 🔥 FIRE = [EXPLOSION]     -> Cataclysmic burst damage + inflicts Burning DoT to target!", (int)modalRec.x + 60, (int)y, 20, (Color){ 231, 76, 60, 255 });
    y += 36;
    DrawText("• 💧 WET + ❄️ COLD = [FROZEN]        -> Immobilizes target completely; skips their next turn!", (int)modalRec.x + 60, (int)y, 20, (Color){ 162, 222, 255, 255 });
    y += 36;
    DrawText("• 🔥 FIRE + ❄️ COLD = [MELT]         -> Superheated steam vaporization bonus damage.", (int)modalRec.x + 60, (int)y, 20, (Color){ 243, 156, 18, 255 });
    y += 36;
    DrawText("• 🛢️ OIL + ⚡ LIGHTNING = [PLASMA]   -> High-voltage plasma strike bonus pierce damage.", (int)modalRec.x + 60, (int)y, 20, (Color){ 155, 89, 182, 255 });
    y += 55;

    // Weather Effects
    DrawText("WEATHER FORECAST MECHANICS (1-3 TURNS AHEAD):", (int)modalRec.x + 50, (int)y, 24, WHITE);
    y += 40;
    DrawText("• 🌧️ RAIN: Inflicts [WET] globally at turn start. +35% Water DMG, dampens Fire.", (int)modalRec.x + 60, (int)y, 19, (Color){ 52, 152, 219, 255 });
    y += 34;
    DrawText("• 🔥 HEATWAVE: +50% Fire DMG to all fire attacks and intensifies combustion.", (int)modalRec.x + 60, (int)y, 19, (Color){ 230, 126, 34, 255 });
    y += 34;
    DrawText("• ⛈️ THUNDERSTORM: Inflicts [WET] globally + 15 Lightning strike on random enemy.", (int)modalRec.x + 60, (int)y, 19, (Color){ 142, 68, 173, 255 });
    y += 34;
    DrawText("• 🌨️ BLIZZARD: Inflicts [COLD] globally. Drenched [WET] targets freeze solid instantly!", (int)modalRec.x + 60, (int)y, 19, (Color){ 129, 236, 236, 255 });
    y += 34;
    DrawText("• 🌪️ GALE: Swirling gusts spread all active elemental debuffs to neighboring combatants.", (int)modalRec.x + 60, (int)y, 19, (Color){ 46, 204, 113, 255 });
    y += 34;
    DrawText("• 🧪 ACID RAIN: Applies [OIL] to everyone, priming the battlefield for explosive combos.", (int)modalRec.x + 60, (int)y, 19, (Color){ 108, 92, 231, 255 });

    // Close button
    Rectangle closeRec = { modalRec.x + modalRec.width * 0.5f - 140, modalRec.y + modalRec.height - 90, 280, 60 };
    if (DrawButton(closeRec, "Close Guide [H]", (Color){ 41, 128, 185, 255 }, (Color){ 52, 152, 219, 255 })) {
        showCheatSheet = false;
    }
}

void UIRenderer::DrawCombatScreen(CombatSystem& combat) {
    DrawBackground(combat.GetWeatherSystem().GetCurrentWeather());
    DrawWeatherForecast(combat.GetWeatherSystem());

    // Draw Particles behind UI
    combat.GetParticleSystem().Draw();

    DrawPlayerCard(combat.GetPlayer(), combat.GetSelectedStance());
    DrawEnemyCards(combat);

    DrawStanceSelector(combat);
    DrawSkillCards(combat);
    DrawExecuteButton(combat);

    DrawCombatLog(combat.GetCombatLog());

    // Draw Reaction Modal if toggled
    if (showCheatSheet) {
        DrawSynergyGuideModal();
    }
}

void UIRenderer::DrawTitleScreen() {
    int w = ScreenConfig::VIRTUAL_WIDTH;
    (void)ScreenConfig::VIRTUAL_HEIGHT;

    DrawBackground(WeatherType::THUNDERSTORM);

    Rectangle titleCard = { (float)w * 0.5f - 650, 180, 1300, 950 };
    DrawCustomCard(titleCard, (Color){ 20, 25, 38, 245 }, (Color){ 241, 196, 15, 255 }, 0.06f);

    int titleY = 240;
    DrawText("ROGUELIKE : ELEMENTAL NEXUS", (int)titleCard.x + 220, titleY, 50, (Color){ 241, 196, 15, 255 });
    DrawText("2560x1440 Fullscreen Tactical Roguelike Prototype", (int)titleCard.x + 360, titleY + 65, 24, (Color){ 180, 190, 210, 255 });

    // Feature highlights
    float y = titleY + 135;
    DrawText("CORE GAMEPLAY ARCHITECTURE:", (int)titleCard.x + 90, (int)y, 26, WHITE);
    y += 45;
    DrawText("1. Weather Forecast Queue: Track upcoming weather 1-3 turns in advance.", (int)titleCard.x + 110, (int)y, 22, (Color){ 52, 152, 219, 255 });
    y += 40;
    DrawText("2. Elemental Chain Reactions: Combine WET + ELEC (Shock), OIL + FIRE (Explosion), etc.", (int)titleCard.x + 110, (int)y, 22, (Color){ 231, 76, 60, 255 });
    y += 40;
    DrawText("3. Stance Strategy: Switch between Attack (+40% DMG), Defense, and Parry.", (int)titleCard.x + 110, (int)y, 22, (Color){ 46, 204, 113, 255 });
    y += 40;
    DrawText("4. Cooldown-based Skills: Pure tactical timing, zero mana RNG.", (int)titleCard.x + 110, (int)y, 22, (Color){ 162, 222, 255, 255 });
    y += 40;
    DrawText("5. Display: Native 2560x1440 Resolution & Fullscreen Toggle [F11].", (int)titleCard.x + 110, (int)y, 22, (Color){ 241, 196, 15, 255 });

    // Start Button
    Rectangle startRec = { titleCard.x + titleCard.width * 0.5f - 240, titleCard.y + 570, 480, 90 };
    DrawButton(startRec, "START EXPEDITION [ENTER]", (Color){ 39, 174, 96, 255 }, (Color){ 46, 204, 113, 255 }, true);

    // Fullscreen Button
    Rectangle fsRec = { titleCard.x + titleCard.width * 0.5f - 240, titleCard.y + 680, 480, 75 };
    if (DrawButton(fsRec, "TOGGLE FULLSCREEN [F11]", (Color){ 108, 92, 231, 255 }, (Color){ 155, 89, 182, 255 }) || IsKeyPressed(KEY_F11)) {
        ScreenConfig::ToggleGameFullscreen();
    }

    DrawText("Press ENTER or Space to Start | Press F11 for Fullscreen | Press H for Guide", (int)titleCard.x + 280, (int)titleCard.y + 820, 22, (Color){ 140, 150, 170, 255 });
}

void UIRenderer::DrawVictoryScreen(const CombatSystem& combat) {
    int w = ScreenConfig::VIRTUAL_WIDTH;
    int h = ScreenConfig::VIRTUAL_HEIGHT;

    DrawRectangle(0, 0, w, h, (Color){ 10, 15, 25, 220 });

    Rectangle card = { (float)w * 0.5f - 500, (float)h * 0.5f - 350, 1000, 700 };
    DrawCustomCard(card, (Color){ 20, 28, 42, 255 }, (Color){ 46, 204, 113, 255 }, 0.06f);

    DrawText("★ WAVE CLEARED! ★", (int)card.x + 300, (int)card.y + 60, 46, (Color){ 46, 204, 113, 255 });
    
    std::string waveMsg = "Completed Wave " + std::to_string(combat.GetCurrentWave()) + " of " + std::to_string(combat.GetMaxWaves());
    DrawText(waveMsg.c_str(), (int)card.x + 350, (int)card.y + 135, 26, (Color){ 180, 190, 210, 255 });

    DrawText("Your elemental mastery shattered the enemy vanguard.", (int)card.x + 190, (int)card.y + 220, 24, WHITE);
    DrawText("+25 HP Restored between waves!", (int)card.x + 310, (int)card.y + 280, 26, (Color){ 52, 152, 219, 255 });

    if (combat.GetCurrentWave() >= combat.GetMaxWaves()) {
        DrawText("🏆 CONGRATULATIONS! YOU CONQUERED ALL WAVES!", (int)card.x + 140, (int)card.y + 360, 28, (Color){ 241, 196, 15, 255 });
    }

    Rectangle nextRec = { card.x + card.width * 0.5f - 220, card.y + 480, 440, 90 };
    const char* btnText = (combat.GetCurrentWave() >= combat.GetMaxWaves()) ? "PLAY AGAIN [ENTER]" : "NEXT WAVE [ENTER]";
    DrawButton(nextRec, btnText, (Color){ 39, 174, 96, 255 }, (Color){ 46, 204, 113, 255 }, true);
}

void UIRenderer::DrawDefeatScreen(const CombatSystem& combat) {
    (void)combat;
    int w = ScreenConfig::VIRTUAL_WIDTH;
    int h = ScreenConfig::VIRTUAL_HEIGHT;

    DrawRectangle(0, 0, w, h, (Color){ 25, 10, 15, 230 });

    Rectangle card = { (float)w * 0.5f - 500, (float)h * 0.5f - 350, 1000, 700 };
    DrawCustomCard(card, (Color){ 32, 20, 25, 255 }, (Color){ 231, 76, 60, 255 }, 0.06f);

    DrawText("☠️ DEFEATED ☠️", (int)card.x + 340, (int)card.y + 65, 48, (Color){ 231, 76, 60, 255 });
    DrawText("The elemental storm proved too overwhelming.", (int)card.x + 230, (int)card.y + 150, 26, (Color){ 200, 180, 185, 255 });

    DrawText("Strategic Tips for Survival:", (int)card.x + 150, (int)card.y + 240, 24, (Color){ 241, 196, 15, 255 });
    DrawText("• Use Parry [E] to reflect incoming status debuffs back to enemies.", (int)card.x + 170, (int)card.y + 290, 22, WHITE);
    DrawText("• Drench enemies with Torrent Slash [1], then Shock with Thunder Strike [3].", (int)card.x + 170, (int)card.y + 335, 22, WHITE);
    DrawText("• Use Glacial Lance [4] on Wet targets to skip their turn.", (int)card.x + 170, (int)card.y + 380, 22, WHITE);

    Rectangle retryRec = { card.x + card.width * 0.5f - 220, card.y + 480, 440, 90 };
    DrawButton(retryRec, "TRY AGAIN [ENTER]", (Color){ 192, 57, 43, 255 }, (Color){ 231, 76, 60, 255 }, true);
}
