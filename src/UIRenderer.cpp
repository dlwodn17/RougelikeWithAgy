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
    DrawRectangleRounded(rec, roundness, 6, bg);
    DrawRectangleRoundedLinesEx(rec, roundness, 6, 2.0f, border);
}

bool UIRenderer::DrawButton(Rectangle rec, const char* text, Color baseColor, Color hoverColor, bool active, bool disabled) {
    Vector2 mousePos = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mousePos, rec) && !disabled;
    bool clicked = hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    Color bg = disabled ? (Color){ 60, 64, 75, 200 } : (active ? hoverColor : (hovered ? hoverColor : baseColor));
    Color border = active ? (Color){ 241, 196, 15, 255 } : (hovered ? WHITE : (Color){ 100, 110, 130, 255 });

    DrawCustomCard(rec, bg, border, 0.15f);

    int fontSize = 18;
    int textWidth = MeasureText(text, fontSize);
    int textX = (int)(rec.x + (rec.width - textWidth) * 0.5f);
    int textY = (int)(rec.y + (rec.height - fontSize) * 0.5f);

    Color textColor = disabled ? (Color){ 140, 145, 155, 255 } : WHITE;
    DrawText(text, textX, textY, fontSize, textColor);

    return clicked;
}

void UIRenderer::DrawBackground(WeatherType weather) {
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    // Dark moody gradient base
    Color topCol = (Color){ 15, 20, 30, 255 };
    Color botCol = (Color){ 25, 30, 45, 255 };

    if (weather == WeatherType::HEATWAVE) {
        topCol = (Color){ 35, 18, 15, 255 };
        botCol = (Color){ 45, 25, 20, 255 };
    } else if (weather == WeatherType::BLIZZARD) {
        topCol = (Color){ 12, 25, 40, 255 };
        botCol = (Color){ 20, 35, 55, 255 };
    } else if (weather == WeatherType::THUNDERSTORM) {
        topCol = (Color){ 20, 15, 35, 255 };
        botCol = (Color){ 28, 20, 45, 255 };
    } else if (weather == WeatherType::RAIN) {
        topCol = (Color){ 15, 25, 35, 255 };
        botCol = (Color){ 20, 35, 48, 255 };
    } else if (weather == WeatherType::ACID_RAIN) {
        topCol = (Color){ 25, 15, 35, 255 };
        botCol = (Color){ 35, 20, 48, 255 };
    }

    DrawRectangleGradientV(0, 0, w, h, topCol, botCol);

    // Subtle arena floor grid
    Color gridColor = (Color){ 255, 255, 255, 8 };
    for (int y = 420; y < h; y += 30) {
        DrawLine(0, y, w, y, gridColor);
    }
}

void UIRenderer::DrawWeatherForecast(const WeatherSystem& weatherSystem) {
    int w = GetScreenWidth();
    Rectangle barRec = { 20, 15, (float)w - 40, 60 };
    DrawCustomCard(barRec, (Color){ 20, 25, 38, 220 }, (Color){ 70, 80, 105, 255 }, 0.12f);

    // Weather label
    DrawText("WEATHER FORECAST", 35, 25, 12, (Color){ 160, 175, 200, 255 });
    DrawText("[1-3 Turns Ahead]", 35, 42, 10, (Color){ 120, 130, 150, 255 });

    // Active Weather Display (Slot 0 - Current Turn)
    WeatherType current = weatherSystem.GetCurrentWeather();
    Rectangle slot0Rec = { 200, 20, 250, 48 };
    Color curColor = WeatherSystem::GetWeatherColor(current);
    DrawCustomCard(slot0Rec, (Color){ 30, 38, 55, 255 }, curColor, 0.15f);

    std::string activeTitle = "ACTIVE: " + std::string(WeatherSystem::GetWeatherIcon(current));
    DrawText(activeTitle.c_str(), 215, 26, 16, curColor);
    DrawText(WeatherSystem::GetWeatherShortDesc(current), 215, 46, 11, (Color){ 210, 220, 235, 255 });

    // Upcoming forecast slots
    const auto& queue = weatherSystem.GetForecastQueue();
    float startX = 470;
    for (size_t i = 0; i < queue.size() && i < 3; ++i) {
        WeatherType nextW = queue[i];
        Rectangle nextRec = { startX + (float)i * 180, 20, 170, 48 };
        DrawCustomCard(nextRec, (Color){ 25, 30, 42, 180 }, (Color){ 60, 70, 90, 200 }, 0.12f);

        std::string turnLabel = "+" + std::to_string(i + 1) + " Turn: " + WeatherSystem::GetWeatherName(nextW);
        DrawText(turnLabel.c_str(), (int)nextRec.x + 10, 27, 12, WeatherSystem::GetWeatherColor(nextW));
        DrawText(WeatherSystem::GetWeatherShortDesc(nextW), (int)nextRec.x + 10, 45, 10, (Color){ 150, 160, 180, 255 });
    }

    // Help / CheatSheet button on far right
    Rectangle helpRec = { (float)w - 180, 23, 145, 42 };
    bool clickedHelp = DrawButton(helpRec, showCheatSheet ? "Hide Guide [H]" : "Reactions Guide [H]", (Color){ 41, 128, 185, 255 }, (Color){ 52, 152, 219, 255 });
    if (clickedHelp || IsKeyPressed(KEY_H) || IsKeyPressed(KEY_TAB)) {
        showCheatSheet = !showCheatSheet;
    }
}

void UIRenderer::DrawHealthBar(Vector2 pos, Vector2 size, int currentHp, int maxHp, int shield, Color fillColor) {
    // Background
    DrawRectangleRounded((Rectangle){ pos.x, pos.y, size.x, size.y }, 0.25f, 4, (Color){ 30, 35, 45, 255 });

    // HP Fill
    float hpPercent = (maxHp > 0) ? std::max(0.0f, std::min(1.0f, (float)currentHp / (float)maxHp)) : 0.0f;
    if (hpPercent > 0.0f) {
        DrawRectangleRounded((Rectangle){ pos.x, pos.y, size.x * hpPercent, size.y }, 0.25f, 4, fillColor);
    }

    // Shield Overlay (if any)
    if (shield > 0) {
        float shieldPercent = (maxHp > 0) ? std::min(1.0f, (float)shield / (float)maxHp) : 0.0f;
        DrawRectangleRounded((Rectangle){ pos.x, pos.y, size.x * shieldPercent, size.y * 0.35f }, 0.2f, 4, (Color){ 100, 200, 255, 220 });
    }

    // Outline
    DrawRectangleRoundedLinesEx((Rectangle){ pos.x, pos.y, size.x, size.y }, 0.25f, 4, 1.5f, (Color){ 80, 90, 110, 255 });

    // Text (HP / MaxHP + Shield)
    std::string hpText = std::to_string(currentHp) + " / " + std::to_string(maxHp);
    if (shield > 0) {
        hpText += " (+" + std::to_string(shield) + " 🛡️)";
    }
    int fontSize = 12;
    int tw = MeasureText(hpText.c_str(), fontSize);
    DrawText(hpText.c_str(), (int)(pos.x + (size.x - tw) * 0.5f), (int)(pos.y + (size.y - fontSize) * 0.5f), fontSize, WHITE);
}

void UIRenderer::DrawStatusBadges(const std::vector<StatusInstance>& statuses, Vector2 startPos) {
    float xOffset = 0;
    for (const auto& st : statuses) {
        if (st.element == Element::NONE || st.duration <= 0) continue;

        Color col = ElementalSystem::GetElementColor(st.element);
        std::string badgeText = std::string(ElementalSystem::GetElementIcon(st.element)) + " (" + std::to_string(st.duration) + ")";
        int fontSize = 11;
        int tw = MeasureText(badgeText.c_str(), fontSize);
        Rectangle badgeRec = { startPos.x + xOffset, startPos.y, (float)tw + 12, 20 };

        DrawRectangleRounded(badgeRec, 0.3f, 4, (Color){ 20, 25, 35, 230 });
        DrawRectangleRoundedLinesEx(badgeRec, 0.3f, 4, 1.5f, col);
        DrawText(badgeText.c_str(), (int)badgeRec.x + 6, (int)badgeRec.y + 4, fontSize, col);

        xOffset += badgeRec.width + 6;
    }
}

void UIRenderer::DrawPlayerCard(const Player& player, StanceType selectedStance) {
    Vector2 offset = player.GetRenderOffset();
    Rectangle cardRec = { 40 + offset.x, 100 + offset.y, 320, 310 };

    // Border glows based on active stance
    Color borderColor = (Color){ 52, 152, 219, 255 };
    if (selectedStance == StanceType::ATTACK) borderColor = (Color){ 231, 76, 60, 255 };
    else if (selectedStance == StanceType::DEFENSE) borderColor = (Color){ 46, 204, 113, 255 };
    else if (selectedStance == StanceType::PARRY) borderColor = (Color){ 241, 196, 15, 255 };

    DrawCustomCard(cardRec, (Color){ 24, 30, 45, 230 }, borderColor, 0.08f);

    // Player Title / Avatar silhouette
    DrawRectangle((int)cardRec.x + 20, (int)cardRec.y + 20, 54, 54, (Color){ 41, 128, 185, 200 });
    DrawRectangleLines((int)cardRec.x + 20, (int)cardRec.y + 20, 54, 54, borderColor);
    DrawText("HERO", (int)cardRec.x + 27, (int)cardRec.y + 38, 14, WHITE);

    DrawText(player.GetName().c_str(), (int)cardRec.x + 85, (int)cardRec.y + 24, 18, WHITE);

    // Stance Badge
    const char* stanceName = "⚔️ ATTACK STANCE (+40% Dmg)";
    Color stanceCol = (Color){ 231, 76, 60, 255 };
    if (selectedStance == StanceType::DEFENSE) {
        stanceName = "🛡️ DEFENSE (+18 Shield, -30% Dmg)";
        stanceCol = (Color){ 46, 204, 113, 255 };
    } else if (selectedStance == StanceType::PARRY) {
        stanceName = "⚡ PARRY (Counter & Reflect)";
        stanceCol = (Color){ 241, 196, 15, 255 };
    }
    DrawText(stanceName, (int)cardRec.x + 85, (int)cardRec.y + 50, 12, stanceCol);

    // HP Bar
    DrawText("HP & SHIELD", (int)cardRec.x + 20, (int)cardRec.y + 90, 12, (Color){ 160, 175, 200, 255 });
    DrawHealthBar((Vector2){ cardRec.x + 20, cardRec.y + 110 }, (Vector2){ 280, 24 }, player.GetHp(), player.GetMaxHp(), player.GetShield(), (Color){ 46, 204, 113, 255 });

    // Active Status Buffers
    DrawText("ELEMENTAL STATUS BUFFER:", (int)cardRec.x + 20, (int)cardRec.y + 150, 12, (Color){ 160, 175, 200, 255 });
    if (player.GetStatusBuffer().empty()) {
        DrawText("(Clean - No debuffs)", (int)cardRec.x + 20, (int)cardRec.y + 175, 12, (Color){ 120, 130, 150, 255 });
    } else {
        DrawStatusBadges(player.GetStatusBuffer(), (Vector2){ cardRec.x + 20, cardRec.y + 175 });
    }

    // Passive Traits Info
    Rectangle traitRec = { cardRec.x + 15, cardRec.y + 220, 290, 75 };
    DrawRectangleRounded(traitRec, 0.1f, 4, (Color){ 18, 22, 34, 200 });
    DrawText("COMBAT SUMMARY:", (int)traitRec.x + 10, (int)traitRec.y + 8, 11, (Color){ 241, 196, 15, 255 });
    DrawText("• Select Skill (1-4) & Target Enemy", (int)traitRec.x + 10, (int)traitRec.y + 26, 11, (Color){ 180, 190, 210, 255 });
    DrawText("• Switch Stance (Q/W/E) to adapt", (int)traitRec.x + 10, (int)traitRec.y + 42, 11, (Color){ 180, 190, 210, 255 });
    DrawText("• Press SPACE to Execute Turn", (int)traitRec.x + 10, (int)traitRec.y + 58, 11, (Color){ 180, 190, 210, 255 });
}

void UIRenderer::DrawEnemyCards(CombatSystem& combat) {
    const auto& enemies = combat.GetEnemies();
    int selectedIdx = combat.GetSelectedTargetIndex();
    Vector2 mousePos = GetMousePosition();

    float startX = 390.0f;
    float cardWidth = 270.0f;
    float spacing = 20.0f;

    if (enemies.size() == 2) {
        startX = 480.0f;
        cardWidth = 340.0f;
        spacing = 30.0f;
    } else if (enemies.size() >= 3) {
        startX = 380.0f;
        cardWidth = 275.0f;
        spacing = 18.0f;
    }

    for (size_t i = 0; i < enemies.size(); ++i) {
        const Enemy& enemy = enemies[i];
        if (!enemy.IsAlive()) continue;

        Vector2 offset = enemy.GetRenderOffset();
        Rectangle rec = { startX + (float)i * (cardWidth + spacing) + offset.x, 100.0f + offset.y, cardWidth, 310.0f };

        bool isSelected = (static_cast<int>(i) == selectedIdx);
        bool isHovered = CheckCollisionPointRec(mousePos, rec);

        if (isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            combat.SelectTarget((int)i);
        }

        // Selection / Hover highlights
        Color border = isSelected ? (Color){ 241, 196, 15, 255 } : (isHovered ? WHITE : (Color){ 65, 75, 95, 255 });
        Color bg = isSelected ? (Color){ 35, 30, 48, 240 } : (Color){ 25, 30, 42, 230 };

        DrawCustomCard(rec, bg, border, 0.08f);

        // Target marker arrow above card
        if (isSelected) {
            DrawText("▼ TARGET", (int)(rec.x + rec.width * 0.5f - 35), (int)rec.y - 20, 14, (Color){ 241, 196, 15, 255 });
        }

        // Enemy Name & Type
        DrawText(enemy.GetName().c_str(), (int)rec.x + 15, (int)rec.y + 16, 17, enemy.GetColor());

        // Intent Bubble above Health
        const Intent& intent = enemy.GetIntent();
        Rectangle intentRec = { rec.x + 15, rec.y + 44, rec.width - 30, 42 };
        Color intentBorder = GetElementBaseColor(intent.element);
        DrawCustomCard(intentRec, (Color){ 18, 20, 30, 220 }, intentBorder, 0.12f);

        std::string intentText = "INTENT: " + intent.name;
        if (intent.type == IntentType::ATTACK) {
            intentText += " (" + std::to_string(intent.value) + " ⚔️)";
        } else if (intent.type == IntentType::DEFEND) {
            intentText += " (+" + std::to_string(intent.value) + " 🛡️)";
        }
        DrawText(intentText.c_str(), (int)intentRec.x + 8, (int)intentRec.y + 6, 12, intentBorder);
        DrawText(intent.desc.c_str(), (int)intentRec.x + 8, (int)intentRec.y + 24, 10, (Color){ 180, 190, 205, 255 });

        // Health Bar
        DrawText("HP & SHIELD", (int)rec.x + 15, (int)rec.y + 98, 11, (Color){ 160, 175, 200, 255 });
        DrawHealthBar((Vector2){ rec.x + 15, rec.y + 116 }, (Vector2){ rec.width - 30, 22 }, enemy.GetHp(), enemy.GetMaxHp(), enemy.GetShield(), (Color){ 231, 76, 60, 255 });

        // Status Buffer
        DrawText("STATUS BUFFER:", (int)rec.x + 15, (int)rec.y + 150, 11, (Color){ 160, 175, 200, 255 });
        if (enemy.GetStatusBuffer().empty()) {
            DrawText("(No active elements)", (int)rec.x + 15, (int)rec.y + 172, 11, (Color){ 120, 130, 150, 255 });
        } else {
            DrawStatusBadges(enemy.GetStatusBuffer(), (Vector2){ rec.x + 15, rec.y + 172 });
        }

        // Frozen / Stun Indicator
        if (enemy.IsFrozen()) {
            Rectangle freezeRec = { rec.x + 15, rec.y + 215, rec.width - 30, 30 };
            DrawRectangleRounded(freezeRec, 0.15f, 4, (Color){ 41, 128, 185, 200 });
            DrawText("🧊 FROZEN (Turn Skipped)", (int)freezeRec.x + 15, (int)freezeRec.y + 8, 12, WHITE);
        }

        // Click to Target Hint
        if (!isSelected) {
            DrawText("[Click to Target]", (int)(rec.x + rec.width * 0.5f - 45), (int)rec.y + 280, 11, (Color){ 120, 130, 150, 255 });
        }
    }
}

void UIRenderer::DrawStanceSelector(CombatSystem& combat) {
    StanceType currentStance = combat.GetSelectedStance();
    bool isInputPhase = (combat.GetPhase() == CombatPhase::PLAYER_INPUT);

    Rectangle panelRec = { 40, 430, 320, 125 };
    DrawCustomCard(panelRec, (Color){ 22, 28, 42, 220 }, (Color){ 65, 75, 95, 255 }, 0.1f);

    DrawText("STANCE SELECTION (Q / W / E)", (int)panelRec.x + 15, (int)panelRec.y + 10, 12, (Color){ 241, 196, 15, 255 });

    // 3 Stance Buttons
    Rectangle atkRec = { panelRec.x + 10, panelRec.y + 32, 95, 80 };
    Rectangle defRec = { panelRec.x + 112, panelRec.y + 32, 95, 80 };
    Rectangle parRec = { panelRec.x + 214, panelRec.y + 32, 95, 80 };

    bool atkActive = (currentStance == StanceType::ATTACK);
    bool defActive = (currentStance == StanceType::DEFENSE);
    bool parActive = (currentStance == StanceType::PARRY);

    if (DrawButton(atkRec, "⚔️ ATK [Q]\n\n+40% Dmg", (Color){ 192, 57, 43, 200 }, (Color){ 231, 76, 60, 255 }, atkActive, !isInputPhase) || (isInputPhase && IsKeyPressed(KEY_Q))) {
        combat.SelectStance(StanceType::ATTACK);
    }

    if (DrawButton(defRec, "🛡️ DEF [W]\n\n+18 Shield", (Color){ 39, 174, 96, 200 }, (Color){ 46, 204, 113, 255 }, defActive, !isInputPhase) || (isInputPhase && IsKeyPressed(KEY_W))) {
        combat.SelectStance(StanceType::DEFENSE);
    }

    if (DrawButton(parRec, "⚡ PAR [E]\n\nCounter", (Color){ 211, 84, 0, 200 }, (Color){ 243, 156, 18, 255 }, parActive, !isInputPhase) || (isInputPhase && IsKeyPressed(KEY_E))) {
        combat.SelectStance(StanceType::PARRY);
    }
}

void UIRenderer::DrawSkillCards(CombatSystem& combat) {
    auto& skills = combat.GetSkillSystem().GetSkills();
    int selectedSkill = combat.GetSelectedSkillIndex();
    bool isInputPhase = (combat.GetPhase() == CombatPhase::PLAYER_INPUT);

    float startX = 380.0f;
    float cardWidth = 165.0f;
    float cardHeight = 125.0f;
    float spacing = 15.0f;

    for (size_t i = 0; i < skills.size(); ++i) {
        const Skill& skill = skills[i];
        Rectangle rec = { startX + (float)i * (cardWidth + spacing), 430.0f, cardWidth, cardHeight };

        bool isSelected = (static_cast<int>(i) == selectedSkill);
        bool isReady = skill.IsReady();
        Vector2 mousePos = GetMousePosition();
        bool hovered = CheckCollisionPointRec(mousePos, rec) && isInputPhase;

        if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            combat.SelectSkill((int)i);
        }

        // Hotkeys 1, 2, 3, 4
        if (isInputPhase && IsKeyPressed(KEY_ONE + (int)i)) {
            combat.SelectSkill((int)i);
        }

        Color border = isSelected ? (Color){ 241, 196, 15, 255 } : (hovered ? WHITE : (Color){ 65, 75, 95, 255 });
        Color bg = isReady ? (isSelected ? (Color){ 35, 42, 60, 255 } : (Color){ 25, 30, 44, 230 }) : (Color){ 20, 22, 30, 200 };

        DrawCustomCard(rec, bg, border, 0.1f);

        // Header: Hotkey + Skill Name
        std::string hotkeyTitle = "[" + std::to_string(i + 1) + "] " + skill.name;
        DrawText(hotkeyTitle.c_str(), (int)rec.x + 8, (int)rec.y + 10, 13, isReady ? skill.themeColor : (Color){ 130, 135, 145, 255 });

        // Element tag badge
        const char* elemName = ElementalSystem::GetElementName(skill.primaryElement);
        DrawText(elemName, (int)rec.x + 8, (int)rec.y + 30, 11, GetElementBaseColor(skill.primaryElement));

        // Damage display
        std::string dmgText = std::to_string(skill.baseDamage) + " DMG";
        DrawText(dmgText.c_str(), (int)rec.x + (int)rec.width - 55, (int)rec.y + 30, 11, (Color){ 241, 196, 15, 255 });

        // Short description
        DrawText(skill.description.c_str(), (int)rec.x + 8, (int)rec.y + 50, 9, (Color){ 180, 190, 205, 255 });

        // Cooldown overlay if on cooldown
        if (!isReady) {
            DrawRectangleRounded(rec, 0.1f, 4, (Color){ 10, 12, 18, 200 });
            std::string cdText = "COOLDOWN: " + std::to_string(skill.currentCooldown) + "T";
            int tw = MeasureText(cdText.c_str(), 14);
            DrawText(cdText.c_str(), (int)(rec.x + (rec.width - tw) * 0.5f), (int)(rec.y + 55), 14, (Color){ 231, 76, 60, 255 });
        }
    }
}

void UIRenderer::DrawExecuteButton(CombatSystem& combat) {
    bool isInputPhase = (combat.GetPhase() == CombatPhase::PLAYER_INPUT);
    Rectangle rec = { 1100, 430, 140, 125 };

    bool clicked = DrawButton(rec, "⚡ EXECUTE\n   TURN\n\n  [SPACE]", (Color){ 39, 174, 96, 220 }, (Color){ 46, 204, 113, 255 }, false, !isInputPhase);

    if ((clicked || (isInputPhase && IsKeyPressed(KEY_SPACE))) && isInputPhase) {
        combat.ExecutePlayerTurn();
    }
}

void UIRenderer::DrawCombatLog(const std::vector<CombatLogEntry>& log) {
    int w = GetScreenWidth();
    Rectangle logRec = { 40, 570, (float)w - 80, 130 };
    DrawCustomCard(logRec, (Color){ 18, 22, 32, 240 }, (Color){ 55, 65, 85, 255 }, 0.08f);

    DrawText("COMBAT ACTION LOG", (int)logRec.x + 15, (int)logRec.y + 8, 11, (Color){ 160, 175, 200, 255 });

    // Show last 5 log entries
    int maxEntries = 5;
    int startIndex = std::max(0, (int)log.size() - maxEntries);
    int lineY = (int)logRec.y + 26;

    for (size_t i = startIndex; i < log.size(); ++i) {
        DrawText(log[i].text.c_str(), (int)logRec.x + 15, lineY, 13, log[i].color);
        lineY += 19;
    }
}

void UIRenderer::DrawSynergyGuideModal() {
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    // Dark backdrop
    DrawRectangle(0, 0, w, h, (Color){ 10, 12, 18, 220 });

    Rectangle modalRec = { (float)w * 0.5f - 360, (float)h * 0.5f - 240, 720, 480 };
    DrawCustomCard(modalRec, (Color){ 24, 30, 45, 255 }, (Color){ 241, 196, 15, 255 }, 0.08f);

    DrawText("★ ELEMENTAL REACTION & WEATHER CHEAT SHEET", (int)modalRec.x + 30, (int)modalRec.y + 25, 18, (Color){ 241, 196, 15, 255 });

    // Reactions Table
    float y = modalRec.y + 65;
    DrawText("CORE ELEMENTAL REACTIONS:", (int)modalRec.x + 30, (int)y, 14, WHITE);
    y += 24;

    DrawText("• 💧 WET + ⚡ LIGHTNING = [SHOCK]  -> High burst + Arcs AoE damage to all enemies!", (int)modalRec.x + 40, (int)y, 12, (Color){ 241, 196, 15, 255 });
    y += 22;
    DrawText("• 🛢️ OIL + 🔥 FIRE = [EXPLOSION]   -> Cataclysmic burst damage + inflicts Burn DoT!", (int)modalRec.x + 40, (int)y, 12, (Color){ 231, 76, 60, 255 });
    y += 22;
    DrawText("• 💧 WET + ❄️ COLD = [FROZEN]      -> Immobilizes target; skips their next turn!", (int)modalRec.x + 40, (int)y, 12, (Color){ 162, 222, 255, 255 });
    y += 22;
    DrawText("• 🔥 FIRE + ❄️ COLD = [MELT]       -> Superheated steam vaporization bonus damage.", (int)modalRec.x + 40, (int)y, 12, (Color){ 243, 156, 18, 255 });
    y += 22;
    DrawText("• 🛢️ OIL + ⚡ LIGHTNING = [PLASMA]  -> High-voltage plasma strike bonus damage.", (int)modalRec.x + 40, (int)y, 12, (Color){ 155, 89, 182, 255 });
    y += 32;

    // Weather Effects
    DrawText("WEATHER FORECAST MECHANICS:", (int)modalRec.x + 30, (int)y, 14, WHITE);
    y += 24;
    DrawText("• 🌧️ RAIN: Inflicts [WET] globally at turn start. +35% Water DMG, extinguishes Fire.", (int)modalRec.x + 40, (int)y, 12, (Color){ 52, 152, 219, 255 });
    y += 20;
    DrawText("• 🔥 HEATWAVE: +50% Fire DMG to all fire attacks and enhances ignition burn.", (int)modalRec.x + 40, (int)y, 12, (Color){ 230, 126, 34, 255 });
    y += 20;
    DrawText("• ⛈️ THUNDERSTORM: Inflicts [WET] globally + 15 Lightning strike on random enemy.", (int)modalRec.x + 40, (int)y, 12, (Color){ 142, 68, 173, 255 });
    y += 20;
    DrawText("• 🌨️ BLIZZARD: Inflicts [COLD] globally. Drenched [WET] targets freeze instantly!", (int)modalRec.x + 40, (int)y, 12, (Color){ 129, 236, 236, 255 });
    y += 20;
    DrawText("• 🌪️ GALE: Spreads all active debuffs on targets to neighboring combatants.", (int)modalRec.x + 40, (int)y, 12, (Color){ 46, 204, 113, 255 });
    y += 20;
    DrawText("• 🧪 ACID RAIN: Applies [OIL] to everyone, priming the field for explosions.", (int)modalRec.x + 40, (int)y, 12, (Color){ 108, 92, 231, 255 });

    // Close button
    Rectangle closeRec = { modalRec.x + modalRec.width * 0.5f - 80, modalRec.y + modalRec.height - 50, 160, 36 };
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
    int w = GetScreenWidth();
    (void)GetScreenHeight();

    DrawBackground(WeatherType::THUNDERSTORM);

    Rectangle titleCard = { (float)w * 0.5f - 350, 100, 700, 500 };
    DrawCustomCard(titleCard, (Color){ 20, 25, 38, 240 }, (Color){ 241, 196, 15, 255 }, 0.08f);

    int titleY = 140;
    DrawText("ROGUELIKE : ELEMENTAL NEXUS", (int)titleCard.x + 100, titleY, 28, (Color){ 241, 196, 15, 255 });
    DrawText("Turn-Based Tactical Roguelike Prototype", (int)titleCard.x + 195, titleY + 38, 14, (Color){ 180, 190, 210, 255 });

    // Feature highlights
    float y = titleY + 80;
    DrawText("KEY GAMEPLAY FEATURES:", (int)titleCard.x + 50, (int)y, 14, WHITE);
    y += 26;
    DrawText("1. Weather Forecast Queue: Track upcoming weather 1-3 turns in advance.", (int)titleCard.x + 60, (int)y, 12, (Color){ 52, 152, 219, 255 });
    y += 22;
    DrawText("2. Elemental Chain Reactions: Combine WET + ELEC (Shock), OIL + FIRE (Explosion), etc.", (int)titleCard.x + 60, (int)y, 12, (Color){ 231, 76, 60, 255 });
    y += 22;
    DrawText("3. Stance Strategy: Switch between Attack (+40% Dmg), Defense, and Parry.", (int)titleCard.x + 60, (int)y, 12, (Color){ 46, 204, 113, 255 });
    y += 22;
    DrawText("4. Cooldown-based Skills: Zero mana RNG; pure tactical timing and elemental mastery.", (int)titleCard.x + 60, (int)y, 12, (Color){ 162, 222, 255, 255 });

    // Start Button
    Rectangle startRec = { titleCard.x + titleCard.width * 0.5f - 140, titleCard.y + 360, 280, 55 };
    DrawButton(startRec, "START EXPEDITION [ENTER]", (Color){ 39, 174, 96, 255 }, (Color){ 46, 204, 113, 255 }, true);

    DrawText("Press ENTER or Space to Start | Press H in battle for Guide", (int)titleCard.x + 160, (int)titleCard.y + 440, 12, (Color){ 140, 150, 170, 255 });
}

void UIRenderer::DrawVictoryScreen(const CombatSystem& combat) {
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    DrawRectangle(0, 0, w, h, (Color){ 10, 15, 25, 210 });

    Rectangle card = { (float)w * 0.5f - 260, (float)h * 0.5f - 180, 520, 360 };
    DrawCustomCard(card, (Color){ 20, 28, 42, 255 }, (Color){ 46, 204, 113, 255 }, 0.08f);

    DrawText("★ WAVE CLEARED! ★", (int)card.x + 130, (int)card.y + 35, 26, (Color){ 46, 204, 113, 255 });
    
    std::string waveMsg = "Completed Wave " + std::to_string(combat.GetCurrentWave()) + " of " + std::to_string(combat.GetMaxWaves());
    DrawText(waveMsg.c_str(), (int)card.x + 160, (int)card.y + 75, 14, (Color){ 180, 190, 210, 255 });

    DrawText("Your elemental mastery shattered the enemy vanguard.", (int)card.x + 75, (int)card.y + 120, 13, WHITE);
    DrawText("+25 HP Restored between waves!", (int)card.x + 140, (int)card.y + 150, 14, (Color){ 52, 152, 219, 255 });

    if (combat.GetCurrentWave() >= combat.GetMaxWaves()) {
        DrawText("🏆 CONGRATULATIONS! YOU CONQUERED ALL WAVES!", (int)card.x + 60, (int)card.y + 190, 15, (Color){ 241, 196, 15, 255 });
    }

    Rectangle nextRec = { card.x + card.width * 0.5f - 120, card.y + 240, 240, 50 };
    const char* btnText = (combat.GetCurrentWave() >= combat.GetMaxWaves()) ? "PLAY AGAIN [ENTER]" : "NEXT WAVE [ENTER]";
    DrawButton(nextRec, btnText, (Color){ 39, 174, 96, 255 }, (Color){ 46, 204, 113, 255 }, true);
}

void UIRenderer::DrawDefeatScreen(const CombatSystem& combat) {
    (void)combat;
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    DrawRectangle(0, 0, w, h, (Color){ 25, 10, 15, 220 });

    Rectangle card = { (float)w * 0.5f - 260, (float)h * 0.5f - 180, 520, 360 };
    DrawCustomCard(card, (Color){ 32, 20, 25, 255 }, (Color){ 231, 76, 60, 255 }, 0.08f);

    DrawText("☠️ DEFEATED ☠️", (int)card.x + 160, (int)card.y + 40, 28, (Color){ 231, 76, 60, 255 });
    DrawText("The elemental storm proved too overwhelming.", (int)card.x + 100, (int)card.y + 90, 14, (Color){ 200, 180, 185, 255 });

    DrawText("Strategic Tip:", (int)card.x + 60, (int)card.y + 140, 13, (Color){ 241, 196, 15, 255 });
    DrawText("• Use Parry [E] to reflect dangerous status debuffs.", (int)card.x + 70, (int)card.y + 165, 12, WHITE);
    DrawText("• Use Torrent Slash -> Thunder Strike for AoE Shock.", (int)card.x + 70, (int)card.y + 190, 12, WHITE);

    Rectangle retryRec = { card.x + card.width * 0.5f - 120, card.y + 245, 240, 50 };
    DrawButton(retryRec, "TRY AGAIN [ENTER]", (Color){ 192, 57, 43, 255 }, (Color){ 231, 76, 60, 255 }, true);
}
