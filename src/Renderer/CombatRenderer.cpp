#include "Renderer/CombatRenderer.hpp"
#include "Renderer/WeatherRenderer.hpp"
#include "Renderer/FontManager.hpp"
#include "Core/Localization.hpp"
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

    std::string s(text);
    size_t newlinePos = s.find('\n');
    if (newlinePos != std::string::npos) {
        // Multi-line text button
        std::stringstream ss(s);
        std::string line;
        std::vector<std::string> lines;
        while (std::getline(ss, line)) {
            lines.push_back(line);
        }
        int totalH = (int)lines.size() * (fontSize + 6);
        int curY = (int)(rec.y + (rec.height - totalH) * 0.5f);
        for (const auto& l : lines) {
            if (!l.empty()) {
                int tw = FontManager::MeasureText(l.c_str(), fontSize);
                int tx = (int)(rec.x + (rec.width - tw) * 0.5f);
                Color textColor = disabled ? (Color){ 140, 145, 155, 255 } : WHITE;
                FontManager::DrawText(l.c_str(), tx, curY, fontSize, textColor);
            }
            curY += fontSize + 6;
        }
    } else {
        int textWidth = FontManager::MeasureText(text, fontSize);
        int textX = (int)(rec.x + (rec.width - textWidth) * 0.5f);
        int textY = (int)(rec.y + (rec.height - fontSize) * 0.5f);
        Color textColor = disabled ? (Color){ 140, 145, 155, 255 } : WHITE;
        FontManager::DrawText(text, textX, textY, fontSize, textColor);
    }

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
        hpText += Localization::IsKorean() ? (" (+" + std::to_string(shield) + " 방어막)") : (" (+" + std::to_string(shield) + " SHIELD)");
    }
    int fontSize = 20;
    int tw = FontManager::MeasureText(hpText.c_str(), fontSize);
    FontManager::DrawText(hpText.c_str(), (int)(pos.x + (size.x - tw) * 0.5f), (int)(pos.y + (size.y - fontSize) * 0.5f), fontSize, WHITE);
}

void CombatRenderer::DrawStatusBadges(const std::vector<StatusInstance>& statuses, Vector2 startPos) {
    float xOffset = 0;
    for (const auto& st : statuses) {
        if (st.element == Element::NONE || st.duration <= 0) continue;

        Color col = ToRaylibColor(ElementalSystem::GetElementColor(st.element));
        std::string badgeText = std::string(Localization::GetElementTag(st.element)) + " (" + std::to_string(st.duration) + "T)";
        int fontSize = 18;
        int tw = FontManager::MeasureText(badgeText.c_str(), fontSize);
        Rectangle badgeRec = { startPos.x + xOffset, startPos.y, (float)tw + 20, 36 };

        DrawRectangleRounded(badgeRec, 0.3f, 6, (Color){ 20, 25, 35, 240 });
        DrawRectangleRoundedLinesEx(badgeRec, 0.3f, 6, 2.0f, col);
        FontManager::DrawText(badgeText.c_str(), (int)badgeRec.x + 10, (int)badgeRec.y + 7, fontSize, col);

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
    FontManager::DrawText(Localization::IsKorean() ? "영웅" : "HERO", (int)cardRec.x + 44, (int)cardRec.y + 62, 22, WHITE);

    std::string heroTitle = Localization::IsKorean() ? "비전 결투사 (Arcane Duelist)" : player.GetName();
    FontManager::DrawText(heroTitle.c_str(), (int)cardRec.x + 140, (int)cardRec.y + 35, 28, WHITE);

    const char* stanceName = Localization::GetStanceName(selectedStance);
    Color stanceCol = (Color){ 231, 76, 60, 255 };
    if (selectedStance == StanceType::DEFENSE) stanceCol = (Color){ 46, 204, 113, 255 };
    else if (selectedStance == StanceType::PARRY) stanceCol = (Color){ 241, 196, 15, 255 };
    FontManager::DrawText(stanceName, (int)cardRec.x + 140, (int)cardRec.y + 78, 20, stanceCol);

    // HP & Shield
    FontManager::DrawText(Localization::IsKorean() ? "생명력(HP) & 방어막" : "HEALTH & SHIELD POINTS", (int)cardRec.x + 30, (int)cardRec.y + 145, 18, (Color){ 160, 175, 200, 255 });
    DrawHealthBar((Vector2){ cardRec.x + 30, cardRec.y + 175 }, (Vector2){ 560, 42 }, player.GetHp(), player.GetMaxHp(), player.GetShield(), (Color){ 46, 204, 113, 255 });

    // Active Elemental Status Buffers
    FontManager::DrawText(Localization::IsKorean() ? "활성 원소 상태이상 버퍼:" : "ACTIVE ELEMENTAL STATUS BUFFER:", (int)cardRec.x + 30, (int)cardRec.y + 245, 18, (Color){ 160, 175, 200, 255 });
    auto statusList = player.GetStatusInstances();
    if (statusList.empty()) {
        FontManager::DrawText(Localization::IsKorean() ? "(상태이상 없음 - 정상 상태)" : "(Clean - No active elemental debuffs)", (int)cardRec.x + 30, (int)cardRec.y + 280, 18, (Color){ 120, 130, 150, 255 });
    } else {
        DrawStatusBadges(statusList, (Vector2){ cardRec.x + 30, cardRec.y + 280 });
    }

    // Tactics & Controls Card
    Rectangle traitRec = { cardRec.x + 25, cardRec.y + 350, 570, 235 };
    DrawRectangleRounded(traitRec, 0.08f, 6, (Color){ 18, 22, 34, 220 });
    if (Localization::IsKorean()) {
        FontManager::DrawText("전투 가이드 & 조작 단축키:", (int)traitRec.x + 20, (int)traitRec.y + 16, 20, (Color){ 241, 196, 15, 255 });
        FontManager::DrawText("- 스킬 선택: [1] 급류 [2] 발화 [3] 낙뢰 [4] 빙하", (int)traitRec.x + 20, (int)traitRec.y + 52, 17, (Color){ 190, 200, 220, 255 });
        FontManager::DrawText("- 태세 변경: [Q] 공격 (+40% 피해) [W] 방어 [E] 패링", (int)traitRec.x + 20, (int)traitRec.y + 82, 17, (Color){ 190, 200, 220, 255 });
        FontManager::DrawText("- 턴 실행: [SPACE] 또는 [ENTER] 키를 눌러 턴 진행", (int)traitRec.x + 20, (int)traitRec.y + 112, 17, (Color){ 190, 200, 220, 255 });
        FontManager::DrawText("- 원소 조합: 수분+전기(감전) / 기름+화염(폭발) / 수분+냉기(빙결)", (int)traitRec.x + 20, (int)traitRec.y + 142, 17, (Color){ 241, 196, 15, 255 });
        FontManager::DrawText("- 단축키: [L] 언어전환 | [O] 해상도/설정 | [H] 도감 | [F11] 전체화면", (int)traitRec.x + 20, (int)traitRec.y + 172, 17, (Color){ 108, 92, 231, 255 });
    } else {
        FontManager::DrawText("COMBAT TACTICS & HOTKEYS:", (int)traitRec.x + 20, (int)traitRec.y + 16, 20, (Color){ 241, 196, 15, 255 });
        FontManager::DrawText("- Skills: [1] Torrent [2] Ignition [3] Thunder [4] Glacial", (int)traitRec.x + 20, (int)traitRec.y + 52, 17, (Color){ 190, 200, 220, 255 });
        FontManager::DrawText("- Stance: [Q] Attack (+40% DMG) [W] Defense [E] Parry", (int)traitRec.x + 20, (int)traitRec.y + 82, 17, (Color){ 190, 200, 220, 255 });
        FontManager::DrawText("- Execution: Press [SPACE] or [ENTER] to Execute Turn", (int)traitRec.x + 20, (int)traitRec.y + 112, 17, (Color){ 190, 200, 220, 255 });
        FontManager::DrawText("- Reactions: Combine WET+ELEC(Shock) / OIL+FIRE(Explosion)", (int)traitRec.x + 20, (int)traitRec.y + 142, 17, (Color){ 241, 196, 15, 255 });
        FontManager::DrawText("- Hotkeys: [L] Language | [O] Options | [H] Guide | [F11] Fullscreen", (int)traitRec.x + 20, (int)traitRec.y + 172, 17, (Color){ 108, 92, 231, 255 });
    }
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
            std::string targetTag = Localization::IsKorean() ? "▼ 지정된 타겟 ▼" : "[v ACTIVE TARGET v]";
            int ttw = FontManager::MeasureText(targetTag.c_str(), 22);
            FontManager::DrawText(targetTag.c_str(), (int)(rec.x + (rec.width - ttw) * 0.5f), (int)rec.y - 30, 22, (Color){ 241, 196, 15, 255 });
        }

        std::string enemyDisplayName = enemy.GetName();
        if (Localization::IsKorean()) {
            if (enemy.GetName() == "Aquamancer Slime") enemyDisplayName = "아쿠아맨서 슬라임 (Slime)";
            else if (enemy.GetName() == "Pyromancer") enemyDisplayName = "파이로맨서 (Pyromancer)";
            else if (enemy.GetName() == "Storm Harpy") enemyDisplayName = "폭풍 하피 (Storm Harpy)";
            else if (enemy.GetName() == "Frost Golem") enemyDisplayName = "서리 골렘 (Frost Golem)";
            else if (enemy.GetName() == "Elemental Archon") enemyDisplayName = "원소의 아콘 (Elemental Archon)";
            else if (enemy.GetName() == "Storm Minion") enemyDisplayName = "폭풍 하수인 (Storm Minion)";
            else if (enemy.GetName() == "Pyro Minion") enemyDisplayName = "화염 하수인 (Pyro Minion)";
        }
        FontManager::DrawText(enemyDisplayName.c_str(), (int)rec.x + 25, (int)rec.y + 22, 26, ToRaylibColor(enemy.GetColor()));

        const Intent& intent = enemy.GetIntent();
        Rectangle intentRec = { rec.x + 25, rec.y + 65, rec.width - 50, 75 };
        Color intentBorder = ToRaylibColor(ElementalSystem::GetElementColor(intent.element));
        DrawCard(intentRec, (Color){ 18, 20, 30, 230 }, intentBorder, 0.10f);

        std::string intentText = Localization::IsKorean() ? ("다음 행동: " + intent.name) : ("INTENT: " + intent.name);
        if (intent.type == IntentType::ATTACK) {
            intentText += Localization::IsKorean() ? (" (" + std::to_string(intent.value) + " 피해)") : (" (" + std::to_string(intent.value) + " DMG)");
        } else if (intent.type == IntentType::DEFEND) {
            intentText += Localization::IsKorean() ? (" (+" + std::to_string(intent.value) + " 방어막)") : (" (+" + std::to_string(intent.value) + " SHIELD)");
        }
        FontManager::DrawText(intentText.c_str(), (int)intentRec.x + 16, (int)intentRec.y + 12, 20, intentBorder);
        FontManager::DrawText(intent.desc.c_str(), (int)intentRec.x + 16, (int)intentRec.y + 42, 16, (Color){ 180, 190, 205, 255 });

        FontManager::DrawText(Localization::IsKorean() ? "생명력 & 방어막" : "HP & SHIELD", (int)rec.x + 25, (int)rec.y + 155, 18, (Color){ 160, 175, 200, 255 });
        DrawHealthBar((Vector2){ rec.x + 25.0f, rec.y + 185.0f }, (Vector2){ rec.width - 50.0f, 42.0f }, enemy.GetHp(), enemy.GetMaxHp(), enemy.GetShield(), (Color){ 231, 76, 60, 255 });

        FontManager::DrawText(Localization::IsKorean() ? "원소 상태이상 버퍼:" : "STATUS EFFECT BUFFER:", (int)rec.x + 25, (int)rec.y + 250, 18, (Color){ 160, 175, 200, 255 });
        auto statusList = enemy.GetStatusInstances();
        if (statusList.empty()) {
            FontManager::DrawText(Localization::IsKorean() ? "(부여된 원소 상태이상 없음)" : "(No active elemental status)", (int)rec.x + 25, (int)rec.y + 285, 18, (Color){ 120, 130, 150, 255 });
        } else {
            DrawStatusBadges(statusList, (Vector2){ rec.x + 25, rec.y + 285 });
        }

        if (enemy.IsFrozen()) {
            Rectangle freezeRec = { rec.x + 25, rec.y + 355, rec.width - 50, 55 };
            DrawRectangleRounded(freezeRec, 0.15f, 6, (Color){ 41, 128, 185, 220 });
            std::string fzText = Localization::IsKorean() ? "[빙결 상태] (다음 턴 행동 불가)" : "[FROZEN] (Next Action Skipped)";
            FontManager::DrawText(fzText.c_str(), (int)freezeRec.x + 25, (int)freezeRec.y + 14, 22, WHITE);
        }

        if (!isSelected) {
            std::string clickText = Localization::IsKorean() ? "[ 마우스로 클릭하여 타겟 지정 ]" : "[ Click to Target Enemy ]";
            int ctw = FontManager::MeasureText(clickText.c_str(), 18);
            FontManager::DrawText(clickText.c_str(), (int)(rec.x + (rec.width - ctw) * 0.5f), (int)rec.y + 560, 18, (Color){ 130, 140, 160, 255 });
        }
    }
}

void CombatRenderer::DrawStancePanel(const CombatSystem& combat) {
    StanceType currentStance = combat.GetSelectedStance();
    bool isInputPhase = (combat.GetPhase() == CombatPhase::PLAYER_INPUT);

    Rectangle panelRec = { GameConstants::STANCE_PANEL_X, GameConstants::STANCE_PANEL_Y,
                          GameConstants::STANCE_PANEL_W, GameConstants::STANCE_PANEL_H };
    DrawCard(panelRec, (Color){ 22, 28, 42, 230 }, (Color){ 65, 75, 95, 255 }, 0.08f);

    std::string header = Localization::IsKorean() ? "전투 태세 선택 [ Q / W / E ]" : "STANCE SELECTION [ Q / W / E ]";
    FontManager::DrawText(header.c_str(), (int)panelRec.x + 25, (int)panelRec.y + 18, 22, (Color){ 241, 196, 15, 255 });

    Rectangle atkRec = { panelRec.x + 20, panelRec.y + 60, 180, 175 };
    Rectangle defRec = { panelRec.x + 220, panelRec.y + 60, 180, 175 };
    Rectangle parRec = { panelRec.x + 420, panelRec.y + 60, 180, 175 };

    bool atkActive = (currentStance == StanceType::ATTACK);
    bool defActive = (currentStance == StanceType::DEFENSE);
    bool parActive = (currentStance == StanceType::PARRY);

    const char* atkLabel = Localization::IsKorean() ? "공격 [Q]\n\n피해 +40%" : "ATK [Q]\n\n+40% DMG";
    const char* defLabel = Localization::IsKorean() ? "방어 [W]\n\n+18 방어막\n피해 -30%" : "DEF [W]\n\n+18 Shield\n-30% DMG";
    const char* parLabel = Localization::IsKorean() ? "패링 [E]\n\n상태 반사\n12 반격" : "PARRY [E]\n\nReflect\nCounter";

    DrawButton(atkRec, atkLabel, (Color){ 192, 57, 43, 220 }, (Color){ 231, 76, 60, 255 }, atkActive, !isInputPhase, 20);
    DrawButton(defRec, defLabel, (Color){ 39, 174, 96, 220 }, (Color){ 46, 204, 113, 255 }, defActive, !isInputPhase, 20);
    DrawButton(parRec, parLabel, (Color){ 211, 84, 0, 220 }, (Color){ 243, 156, 18, 255 }, parActive, !isInputPhase, 20);
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

        std::string skillDisplayName = skill.GetName();
        if (Localization::IsKorean()) {
            if (skill.GetId() == "torrent_slash") skillDisplayName = "급류 베기 (Torrent)";
            else if (skill.GetId() == "ignition_flask") skillDisplayName = "발화 플라스크 (Ignite)";
            else if (skill.GetId() == "thunder_strike") skillDisplayName = "낙뢰 강타 (Thunder)";
            else if (skill.GetId() == "glacial_lance") skillDisplayName = "빙하의 창 (Glacial)";
        }

        std::string hotkeyTitle = "[" + std::to_string(i + 1) + "] " + skillDisplayName;
        Color themeCol = ToRaylibColor(skill.GetThemeColor());
        FontManager::DrawText(hotkeyTitle.c_str(), (int)rec.x + 18, (int)rec.y + 16, 22, isReady ? themeCol : (Color){ 130, 135, 145, 255 });

        Element effElem = skill.GetEffectiveElement();
        const char* elemName = Localization::GetElementName(effElem);
        FontManager::DrawText(elemName, (int)rec.x + 18, (int)rec.y + 52, 18, ToRaylibColor(ElementalSystem::GetElementColor(effElem)));

        std::string dmgText = std::to_string(skill.GetEffectiveDamage()) + (Localization::IsKorean() ? " 피해" : " DMG");
        FontManager::DrawText(dmgText.c_str(), (int)rec.x + (int)rec.width - 110, (int)rec.y + 52, 20, (Color){ 241, 196, 15, 255 });

        FontManager::DrawText(skill.GetDescription().c_str(), (int)rec.x + 18, (int)rec.y + 92, 15, (Color){ 180, 190, 205, 255 });

        if (!isReady) {
            DrawRectangleRounded(rec, 0.08f, 6, (Color){ 10, 12, 18, 220 });
            std::string cdText = Localization::IsKorean() ? ("재사용 대기: " + std::to_string(skill.GetCurrentCooldown()) + "턴") : ("COOLDOWN: " + std::to_string(skill.GetCurrentCooldown()) + "T");
            int tw = FontManager::MeasureText(cdText.c_str(), 24);
            FontManager::DrawText(cdText.c_str(), (int)(rec.x + (rec.width - tw) * 0.5f), (int)(rec.y + 115), 24, (Color){ 231, 76, 60, 255 });
        }
    }
}

void CombatRenderer::DrawExecuteButton(const CombatSystem& combat) {
    bool isInputPhase = (combat.GetPhase() == CombatPhase::PLAYER_INPUT);
    Rectangle rec = { GameConstants::EXECUTE_BTN_X, GameConstants::EXECUTE_BTN_Y,
                      GameConstants::EXECUTE_BTN_W, GameConstants::EXECUTE_BTN_H };
    const char* execLabel = Localization::IsKorean() ? "턴 실행\n\n[SPACE]" : "EXECUTE\n TURN\n\n[SPACE]";
    DrawButton(rec, execLabel, (Color){ 39, 174, 96, 220 }, (Color){ 46, 204, 113, 255 }, false, !isInputPhase, 24);
}

void CombatRenderer::DrawLogPanel(const std::vector<CombatLogEntry>& log) {
    Rectangle logRec = { GameConstants::LOG_PANEL_X, GameConstants::LOG_PANEL_Y,
                         GameConstants::LOG_PANEL_W, GameConstants::LOG_PANEL_H };
    DrawCard(logRec, (Color){ 18, 22, 32, 245 }, (Color){ 55, 65, 85, 255 }, 0.06f);

    std::string header = Localization::IsKorean() ? "전투 행동 로그" : "COMBAT ACTION LOG";
    FontManager::DrawText(header.c_str(), (int)logRec.x + 25, (int)logRec.y + 15, 20, (Color){ 160, 175, 200, 255 });

    int maxEntries = 8;
    int startIndex = std::max(0, (int)log.size() - maxEntries);
    int lineY = (int)logRec.y + 48;

    for (size_t i = startIndex; i < log.size(); ++i) {
        FontManager::DrawText(log[i].text.c_str(), (int)logRec.x + 25, lineY, 20, ToRaylibColor(log[i].color));
        lineY += 28;
    }
}
