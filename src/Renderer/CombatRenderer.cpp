#include "Renderer/CombatRenderer.hpp"
#include "Renderer/WeatherRenderer.hpp"
#include <iomanip>
#include <sstream>

void CombatRenderer::DrawCard(Rectangle rec, Color bg, Color border, float roundness) {
    DrawRectangleRounded(rec, roundness, 8, bg);
    DrawRectangleRoundedLinesEx(rec, roundness, 8, 3.0f, border);
}

bool CombatRenderer::DrawButton(Rectangle rec, const char* text, Color baseColor, Color hoverColor, bool active, bool disabled, int fontSize) {
    Vector2 mousePos = ScreenConfig::GetVirtualMousePosition();
    bool hovered = CheckCollisionPointRec(mousePos, rec) && !disabled;
    bool clicked = hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);

    Color bg = disabled ? (Color){ 50, 54, 65, 220 } : (active ? hoverColor : (hovered ? hoverColor : baseColor));
    Color border = active ? (Color){ 241, 196, 15, 255 } : (hovered ? WHITE : (Color){ 100, 115, 140, 255 });

    DrawCard(rec, bg, border, 0.12f);

    int textWidth = MeasureText(text, fontSize);
    int textX = (int)(rec.x + (rec.width - textWidth) * 0.5f);
    int textY = (int)(rec.y + (rec.height - fontSize) * 0.5f);

    Color textColor = disabled ? (Color){ 140, 145, 155, 255 } : WHITE;
    DrawText(text, textX, textY, fontSize, textColor);

    return clicked;
}

void CombatRenderer::DrawHealthBar(Vector2 pos, Vector2 size, int currentHp, int maxHp, int shield, Color fillColor) {
    DrawRectangleRounded((Rectangle){ pos.x, pos.y, size.x, size.y }, 0.25f, 6, (Color){ 30, 35, 45, 255 });

    float hpPercent = (maxHp > 0) ? std::max(0.0f, std::min(1.0f, (float)currentHp / (float)maxHp)) : 0.0f;
    if (hpPercent > 0.0f) {
        DrawRectangleRounded((Rectangle){ pos.x, pos.y, size.x * hpPercent, size.y }, 0.25f, 6, fillColor);
    }

    if (shield > 0) {
        float shieldPercent = (maxHp > 0) ? std::min(1.0f, (float)shield / (float)maxHp) : 0.0f;
        DrawRectangleRounded((Rectangle){ pos.x, pos.y, size.x * shieldPercent, size.y * 0.4f }, 0.2f, 6, (Color){ 100, 200, 255, 220 });
    }

    DrawRectangleRoundedLinesEx((Rectangle){ pos.x, pos.y, size.x, size.y }, 0.25f, 6, 2.0f, (Color){ 80, 90, 110, 255 });

    std::string hpText = std::to_string(currentHp) + " / " + std::to_string(maxHp);
    if (shield > 0) {
        hpText += " (+" + std::to_string(shield) + " SHIELD)";
    }
    int fontSize = 20;
    int tw = MeasureText(hpText.c_str(), fontSize);
    DrawText(hpText.c_str(), (int)(pos.x + (size.x - tw) * 0.5f), (int)(pos.y + (size.y - fontSize) * 0.5f), fontSize, WHITE);
}

void CombatRenderer::DrawStatusBadges(const std::vector<StatusInstance>& statuses, Vector2 startPos) {
    float xOffset = 0;
    for (const auto& st : statuses) {
        if (st.element == Element::NONE || st.duration <= 0) continue;

        Color col = ToRaylibColor(ElementalSystem::GetElementColor(st.element));
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

void CombatRenderer::DrawBackground(WeatherType weather) {
    WeatherRenderer::DrawWeatherBackground(weather);
}

void CombatRenderer::DrawWeatherForecast(const WeatherSystem& weatherSystem) {
    WeatherRenderer::DrawForecastPanel(weatherSystem);
}

void CombatRenderer::DrawPlayerPanel(const Player& player, StanceType selectedStance) {
    Vec2 offset = player.GetRenderOffset();
    Rectangle cardRec = { GameConstants::PLAYER_CARD_X + offset.x, GameConstants::PLAYER_CARD_Y + offset.y,
                          GameConstants::PLAYER_CARD_W, GameConstants::PLAYER_CARD_H };

    Color borderColor = (Color){ 52, 152, 219, 255 };
    if (selectedStance == StanceType::ATTACK) borderColor = (Color){ 231, 76, 60, 255 };
    else if (selectedStance == StanceType::DEFENSE) borderColor = (Color){ 46, 204, 113, 255 };
    else if (selectedStance == StanceType::PARRY) borderColor = (Color){ 241, 196, 15, 255 };

    DrawCard(cardRec, (Color){ 24, 30, 45, 240 }, borderColor, 0.06f);

    // Hero Avatar Badge
    DrawRectangle((int)cardRec.x + 30, (int)cardRec.y + 30, 90, 90, (Color){ 41, 128, 185, 220 });
    DrawRectangleLinesEx((Rectangle){ cardRec.x + 30, cardRec.y + 30, 90, 90 }, 2.5f, borderColor);
    DrawText("HERO", (int)cardRec.x + 46, (int)cardRec.y + 64, 20, WHITE);

    DrawText(player.GetName().c_str(), (int)cardRec.x + 140, (int)cardRec.y + 35, 30, WHITE);

    const char* stanceName = "[ATK] ATTACK STANCE (+40% DMG)";
    Color stanceCol = (Color){ 231, 76, 60, 255 };
    if (selectedStance == StanceType::DEFENSE) {
        stanceName = "[DEF] DEFENSE (+18 Shield, -30% DMG)";
        stanceCol = (Color){ 46, 204, 113, 255 };
    } else if (selectedStance == StanceType::PARRY) {
        stanceName = "[PARRY] PARRY (Counter & Reflect)";
        stanceCol = (Color){ 241, 196, 15, 255 };
    }
    DrawText(stanceName, (int)cardRec.x + 140, (int)cardRec.y + 78, 20, stanceCol);

    // HP & Shield
    DrawText("HEALTH & SHIELD POINTS", (int)cardRec.x + 30, (int)cardRec.y + 145, 18, (Color){ 160, 175, 200, 255 });
    DrawHealthBar((Vector2){ cardRec.x + 30, cardRec.y + 175 }, (Vector2){ 560, 42 }, player.GetHp(), player.GetMaxHp(), player.GetShield(), (Color){ 46, 204, 113, 255 });

    // Active Elemental Status Buffers
    DrawText("ACTIVE ELEMENTAL STATUS BUFFER:", (int)cardRec.x + 30, (int)cardRec.y + 245, 18, (Color){ 160, 175, 200, 255 });
    auto statusList = player.GetStatusInstances();
    if (statusList.empty()) {
        DrawText("(Clean - No active elemental debuffs)", (int)cardRec.x + 30, (int)cardRec.y + 280, 18, (Color){ 120, 130, 150, 255 });
    } else {
        DrawStatusBadges(statusList, (Vector2){ cardRec.x + 30, cardRec.y + 280 });
    }

    // Tactics & Controls Card
    Rectangle traitRec = { cardRec.x + 25, cardRec.y + 350, 570, 235 };
    DrawRectangleRounded(traitRec, 0.08f, 6, (Color){ 18, 22, 34, 220 });
    DrawText("COMBAT TACTICS & HOTKEYS:", (int)traitRec.x + 20, (int)traitRec.y + 18, 20, (Color){ 241, 196, 15, 255 });
    DrawText("- Skills: [1] Torrent [2] Ignition [3] Thunder [4] Glacial", (int)traitRec.x + 20, (int)traitRec.y + 55, 17, (Color){ 190, 200, 220, 255 });
    DrawText("- Stance: [Q] Attack (+40% DMG) [W] Defense [E] Parry", (int)traitRec.x + 20, (int)traitRec.y + 85, 17, (Color){ 190, 200, 220, 255 });
    DrawText("- Execution: Press [SPACE] or [ENTER] to Execute Turn", (int)traitRec.x + 20, (int)traitRec.y + 115, 17, (Color){ 190, 200, 220, 255 });
    DrawText("- Reactions: Combine WET + ELEC (Shock) / OIL + FIRE (Explosion)", (int)traitRec.x + 20, (int)traitRec.y + 145, 17, (Color){ 241, 196, 15, 255 });
    DrawText("- Press [O] for Options / Resolution | [F11] Fullscreen", (int)traitRec.x + 20, (int)traitRec.y + 175, 17, (Color){ 108, 92, 231, 255 });
}

void CombatRenderer::DrawEnemyPanel(const CombatSystem& combat) {
    const auto& enemies = combat.GetEnemies();
    int selectedIdx = combat.GetSelectedTargetIndex();
    Vector2 mousePos = ScreenConfig::GetVirtualMousePosition();

    float startX = GameConstants::ENEMY_START_X;
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

        Vec2 offset = enemy.GetRenderOffset();
        Rectangle rec = { startX + (float)i * (cardWidth + spacing) + offset.x,
                          GameConstants::ENEMY_CARD_Y + offset.y, cardWidth, GameConstants::ENEMY_CARD_H };

        bool isSelected = (static_cast<int>(i) == selectedIdx);
        bool isHovered = CheckCollisionPointRec(mousePos, rec);

        Color border = isSelected ? (Color){ 241, 196, 15, 255 } : (isHovered ? WHITE : (Color){ 65, 75, 95, 255 });
        Color bg = isSelected ? (Color){ 35, 30, 50, 245 } : (Color){ 25, 30, 42, 235 };

        DrawCard(rec, bg, border, 0.06f);

        if (isSelected) {
            DrawText("[v ACTIVE TARGET v]", (int)(rec.x + rec.width * 0.5f - 110), (int)rec.y - 30, 22, (Color){ 241, 196, 15, 255 });
        }

        DrawText(enemy.GetName().c_str(), (int)rec.x + 25, (int)rec.y + 25, 28, ToRaylibColor(enemy.GetColor()));

        const Intent& intent = enemy.GetIntent();
        Rectangle intentRec = { rec.x + 25, rec.y + 70, rec.width - 50, 75 };
        Color intentBorder = ToRaylibColor(ElementalSystem::GetElementColor(intent.element));
        DrawCard(intentRec, (Color){ 18, 20, 30, 230 }, intentBorder, 0.10f);

        std::string intentText = "INTENT: " + intent.name;
        if (intent.type == IntentType::ATTACK) {
            intentText += " (" + std::to_string(intent.value) + " DMG)";
        } else if (intent.type == IntentType::DEFEND) {
            intentText += " (+" + std::to_string(intent.value) + " SHIELD)";
        }
        DrawText(intentText.c_str(), (int)intentRec.x + 16, (int)intentRec.y + 12, 20, intentBorder);
        DrawText(intent.desc.c_str(), (int)intentRec.x + 16, (int)intentRec.y + 42, 16, (Color){ 180, 190, 205, 255 });

        DrawText("HP & SHIELD", (int)rec.x + 25, (int)rec.y + 160, 18, (Color){ 160, 175, 200, 255 });
        DrawHealthBar((Vector2){ rec.x + 25.0f, rec.y + 190.0f }, (Vector2){ rec.width - 50.0f, 42.0f }, enemy.GetHp(), enemy.GetMaxHp(), enemy.GetShield(), (Color){ 231, 76, 60, 255 });

        DrawText("STATUS EFFECT BUFFER:", (int)rec.x + 25, (int)rec.y + 255, 18, (Color){ 160, 175, 200, 255 });
        auto statusList = enemy.GetStatusInstances();
        if (statusList.empty()) {
            DrawText("(No active elemental status)", (int)rec.x + 25, (int)rec.y + 290, 18, (Color){ 120, 130, 150, 255 });
        } else {
            DrawStatusBadges(statusList, (Vector2){ rec.x + 25, rec.y + 290 });
        }

        if (enemy.IsFrozen()) {
            Rectangle freezeRec = { rec.x + 25, rec.y + 360, rec.width - 50, 55 };
            DrawRectangleRounded(freezeRec, 0.15f, 6, (Color){ 41, 128, 185, 220 });
            DrawText("[FROZEN] (Next Action Skipped)", (int)freezeRec.x + 30, (int)freezeRec.y + 15, 22, WHITE);
        }

        if (!isSelected) {
            DrawText("[ Click to Target Enemy ]", (int)(rec.x + rec.width * 0.5f - 110), (int)rec.y + 560, 18, (Color){ 130, 140, 160, 255 });
        }
    }
}

void CombatRenderer::DrawStancePanel(const CombatSystem& combat) {
    StanceType currentStance = combat.GetSelectedStance();
    bool isInputPhase = (combat.GetPhase() == CombatPhase::PLAYER_INPUT);

    Rectangle panelRec = { GameConstants::STANCE_PANEL_X, GameConstants::STANCE_PANEL_Y,
                          GameConstants::STANCE_PANEL_W, GameConstants::STANCE_PANEL_H };
    DrawCard(panelRec, (Color){ 22, 28, 42, 230 }, (Color){ 65, 75, 95, 255 }, 0.08f);

    DrawText("STANCE SELECTION [ Q / W / E ]", (int)panelRec.x + 25, (int)panelRec.y + 20, 22, (Color){ 241, 196, 15, 255 });

    Rectangle atkRec = { panelRec.x + 20, panelRec.y + 65, 180, 170 };
    Rectangle defRec = { panelRec.x + 220, panelRec.y + 65, 180, 170 };
    Rectangle parRec = { panelRec.x + 420, panelRec.y + 65, 180, 170 };

    bool atkActive = (currentStance == StanceType::ATTACK);
    bool defActive = (currentStance == StanceType::DEFENSE);
    bool parActive = (currentStance == StanceType::PARRY);

    DrawButton(atkRec, "ATK [Q]\n\n+40% DMG", (Color){ 192, 57, 43, 220 }, (Color){ 231, 76, 60, 255 }, atkActive, !isInputPhase);
    DrawButton(defRec, "DEF [W]\n\n+18 Shield\n-30% DMG", (Color){ 39, 174, 96, 220 }, (Color){ 46, 204, 113, 255 }, defActive, !isInputPhase);
    DrawButton(parRec, "PARRY [E]\n\nReflect\nCounter", (Color){ 211, 84, 0, 220 }, (Color){ 243, 156, 18, 255 }, parActive, !isInputPhase);
}

void CombatRenderer::DrawSkillPanel(const CombatSystem& combat) {
    auto& skills = combat.GetSkillSystem().GetSkills();
    int selectedSkill = combat.GetSelectedSkillIndex();
    bool isInputPhase = (combat.GetPhase() == CombatPhase::PLAYER_INPUT);

    float spacing = 25.0f;
    Vector2 mousePos = ScreenConfig::GetVirtualMousePosition();

    for (size_t i = 0; i < skills.size(); ++i) {
        const Skill& skill = skills[i];
        Rectangle rec = { GameConstants::SKILL_TRAY_X + (float)i * (GameConstants::SKILL_CARD_W + spacing),
                          GameConstants::SKILL_TRAY_Y, GameConstants::SKILL_CARD_W, GameConstants::SKILL_CARD_H };

        bool isSelected = (static_cast<int>(i) == selectedSkill);
        bool isReady = skill.IsReady();
        bool hovered = CheckCollisionPointRec(mousePos, rec) && isInputPhase;

        Color border = isSelected ? (Color){ 241, 196, 15, 255 } : (hovered ? WHITE : (Color){ 65, 75, 95, 255 });
        Color bg = isReady ? (isSelected ? (Color){ 35, 42, 60, 255 } : (Color){ 25, 30, 44, 235 }) : (Color){ 20, 22, 30, 210 };

        DrawCard(rec, bg, border, 0.08f);

        std::string hotkeyTitle = "[" + std::to_string(i + 1) + "] " + skill.GetName();
        Color themeCol = ToRaylibColor(skill.GetThemeColor());
        DrawText(hotkeyTitle.c_str(), (int)rec.x + 18, (int)rec.y + 18, 22, isReady ? themeCol : (Color){ 130, 135, 145, 255 });

        Element effElem = skill.GetEffectiveElement();
        const char* elemName = ElementalSystem::GetElementName(effElem);
        DrawText(elemName, (int)rec.x + 18, (int)rec.y + 55, 18, ToRaylibColor(ElementalSystem::GetElementColor(effElem)));

        std::string dmgText = std::to_string(skill.GetEffectiveDamage()) + " DMG";
        DrawText(dmgText.c_str(), (int)rec.x + (int)rec.width - 110, (int)rec.y + 55, 20, (Color){ 241, 196, 15, 255 });

        DrawText(skill.GetDescription().c_str(), (int)rec.x + 18, (int)rec.y + 95, 15, (Color){ 180, 190, 205, 255 });

        if (!isReady) {
            DrawRectangleRounded(rec, 0.08f, 6, (Color){ 10, 12, 18, 220 });
            std::string cdText = "COOLDOWN: " + std::to_string(skill.GetCurrentCooldown()) + "T";
            int tw = MeasureText(cdText.c_str(), 26);
            DrawText(cdText.c_str(), (int)(rec.x + (rec.width - tw) * 0.5f), (int)(rec.y + 115), 26, (Color){ 231, 76, 60, 255 });
        }
    }
}

void CombatRenderer::DrawExecuteButton(const CombatSystem& combat) {
    bool isInputPhase = (combat.GetPhase() == CombatPhase::PLAYER_INPUT);
    Rectangle rec = { GameConstants::EXECUTE_BTN_X, GameConstants::EXECUTE_BTN_Y,
                      GameConstants::EXECUTE_BTN_W, GameConstants::EXECUTE_BTN_H };
    DrawButton(rec, "EXECUTE\n TURN\n\n[SPACE]", (Color){ 39, 174, 96, 220 }, (Color){ 46, 204, 113, 255 }, false, !isInputPhase);
}

void CombatRenderer::DrawLogPanel(const std::vector<CombatLogEntry>& log) {
    Rectangle logRec = { GameConstants::LOG_PANEL_X, GameConstants::LOG_PANEL_Y,
                         GameConstants::LOG_PANEL_W, GameConstants::LOG_PANEL_H };
    DrawCard(logRec, (Color){ 18, 22, 32, 245 }, (Color){ 55, 65, 85, 255 }, 0.06f);

    DrawText("COMBAT ACTION LOG", (int)logRec.x + 25, (int)logRec.y + 15, 20, (Color){ 160, 175, 200, 255 });

    int maxEntries = 8;
    int startIndex = std::max(0, (int)log.size() - maxEntries);
    int lineY = (int)logRec.y + 48;

    for (size_t i = startIndex; i < log.size(); ++i) {
        DrawText(log[i].text.c_str(), (int)logRec.x + 25, lineY, 20, ToRaylibColor(log[i].color));
        lineY += 28;
    }
}
