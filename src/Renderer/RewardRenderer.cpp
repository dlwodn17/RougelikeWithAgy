#include "Renderer/RewardRenderer.hpp"
#include "Renderer/FontManager.hpp"
#include "Renderer/CombatRenderer.hpp"
#include "Core/Localization.hpp"

void RewardRenderer::DrawRuneCard(Rectangle rec, const Rune& rune, int runeIndex, bool isHovered, bool isSelected) {
    Color themeColor = ToRaylibColor(rune.runeColor);
    Color borderColor = isSelected ? (Color){ 241, 196, 15, 255 } : (isHovered ? WHITE : themeColor);
    Color bgColor = isSelected ? (Color){ 35, 42, 60, 250 } : (isHovered ? (Color){ 28, 34, 50, 245 } : (Color){ 20, 25, 38, 235 });

    CombatRenderer::DrawCard(rec, bgColor, borderColor, 0.08f);

    // 1. Rune Index & Category Tag
    std::string catBadge = "[" + std::to_string(runeIndex + 1) + "] " + std::string(rune.GetCategoryName());
    FontManager::DrawText(catBadge.c_str(), (int)rec.x + 25, (int)rec.y + 24, FontSize::BODY_SMALL, themeColor);

    // 2. Rune Name
    FontManager::DrawText(rune.GetName().c_str(), (int)rec.x + 25, (int)rec.y + 55, FontSize::CARD_TITLE + 2, WHITE);

    // 3. Modifier Attribute Badges Box
    Rectangle badgeBox = { rec.x + 20, rec.y + 105, rec.width - 40, 95 };
    DrawRectangleRounded(badgeBox, 0.12f, 6, (Color){ 14, 18, 28, 220 });
    DrawRectangleRoundedLinesEx(badgeBox, 0.12f, 6, 1.5f, (Color){ 65, 75, 95, 255 });

    float tagX = badgeBox.x + 15;
    float tagY = badgeBox.y + 12;

    if (rune.overrideElement != Element::NONE) {
        std::string elemTag = Localization::IsKorean() ? 
            ("원소 변환: " + std::string(Localization::GetElementName(rune.overrideElement))) :
            ("Convert: " + std::string(Localization::GetElementName(rune.overrideElement)));
        Color elemColor = ToRaylibColor(GetElementColorRGBA(rune.overrideElement));
        FontManager::DrawText(elemTag.c_str(), (int)tagX, (int)tagY, FontSize::BODY_SMALL, elemColor);
        tagY += 28;
    }
    if (rune.damageMultiplier != 1.0f || rune.bonusDamage != 0) {
        std::string dmgTag = Localization::IsKorean() ? "피해량: " : "Damage: ";
        if (rune.bonusDamage > 0) dmgTag += "+" + std::to_string(rune.bonusDamage) + " ";
        if (rune.damageMultiplier > 1.0f) dmgTag += "+" + std::to_string(static_cast<int>((rune.damageMultiplier - 1.0f) * 100)) + "%";
        else if (rune.damageMultiplier < 1.0f) dmgTag += "-" + std::to_string(static_cast<int>((1.0f - rune.damageMultiplier) * 100)) + "%";
        FontManager::DrawText(dmgTag.c_str(), (int)tagX, (int)tagY, FontSize::BODY_SMALL, (Color){ 241, 196, 15, 255 });
        tagY += 28;
    }
    if (rune.cooldownDelta != 0) {
        std::string cdTag = Localization::IsKorean() ? 
            ("쿨다운: " + (rune.cooldownDelta > 0 ? ("+" + std::to_string(rune.cooldownDelta) + "턴") : (std::to_string(rune.cooldownDelta) + "턴"))) :
            ("Cooldown: " + (rune.cooldownDelta > 0 ? ("+" + std::to_string(rune.cooldownDelta) + "T") : (std::to_string(rune.cooldownDelta) + "T")));
        Color cdCol = (rune.cooldownDelta < 0) ? (Color){ 46, 204, 113, 255 } : (Color){ 231, 76, 60, 255 };
        FontManager::DrawText(cdTag.c_str(), (int)tagX, (int)tagY, FontSize::BODY_SMALL, cdCol);
    }
    if (rune.addSecondaryElement != Element::NONE) {
        std::string secTag = Localization::IsKorean() ? 
            ("추가 원소 주입: " + std::string(Localization::GetElementName(rune.addSecondaryElement))) :
            ("Infuse: " + std::string(Localization::GetElementName(rune.addSecondaryElement)));
        FontManager::DrawText(secTag.c_str(), (int)tagX, (int)tagY, FontSize::BODY_SMALL, ToRaylibColor(GetElementColorRGBA(rune.addSecondaryElement)));
    }
    if (rune.freezeWetTarget) {
        std::string frzTag = Localization::IsKorean() ? "특수: [수분] 대상 즉시 빙결" : "Special: Flash-Freeze [WET] targets";
        FontManager::DrawText(frzTag.c_str(), (int)tagX, (int)tagY, FontSize::BODY_SMALL, (Color){ 162, 222, 255, 255 });
    }
    if (rune.chainAoEOnHit) {
        std::string aoeTag = Localization::IsKorean() ? "특수: 모든 적에게 감전 아크 전파" : "Special: Shockwave Arcs to all alive enemies";
        FontManager::DrawText(aoeTag.c_str(), (int)tagX, (int)tagY, FontSize::BODY_SMALL, (Color){ 241, 196, 15, 255 });
    }
    if (rune.shieldScalingDamage) {
        std::string shdTag = Localization::IsKorean() ? "특수: 자신의 방어막만큼 피해 추가" : "Special: Damage scales with Player Shield";
        FontManager::DrawText(shdTag.c_str(), (int)tagX, (int)tagY, FontSize::BODY_SMALL, (Color){ 52, 152, 219, 255 });
    }

    // 4. Detailed Description Text
    Rectangle descRec = { rec.x + 20, rec.y + 215, rec.width - 40, 200 };
    FontManager::DrawText(rune.GetDescription().c_str(), (int)descRec.x + 5, (int)descRec.y + 5, FontSize::BODY_REGULAR - 1, (Color){ 200, 210, 230, 255 });

    // 5. Select Button
    Rectangle btnRec = { rec.x + 25, rec.y + rec.height - 75, rec.width - 50, 55 };
    const char* btnLabel = Localization::IsKorean() ? "이 룬 선택하기" : "Select Rune";
    CombatRenderer::DrawButton(btnRec, btnLabel, (Color){ 39, 174, 96, 220 }, (Color){ 46, 204, 113, 255 }, isSelected, false, FontSize::BUTTON_MEDIUM);
}

void RewardRenderer::DrawSocketingModal(const RewardSystem& rewardSystem, const SkillSystem& skillSystem) {
    const Rune* chosenRune = rewardSystem.GetSelectedRune();
    if (!chosenRune) return;

    Vector2 mousePos = ScreenConfig::GetVirtualMousePosition();

    // Dark backdrop for socketing modal
    DrawRectangle(0, 0, ScreenConfig::VIRTUAL_WIDTH, ScreenConfig::VIRTUAL_HEIGHT, (Color){ 8, 10, 16, 225 });

    Rectangle modalRec = { (float)ScreenConfig::VIRTUAL_WIDTH * 0.5f - 850.0f, (float)ScreenConfig::VIRTUAL_HEIGHT * 0.5f - 460.0f, 1700.0f, 920.0f };
    CombatRenderer::DrawCard(modalRec, (Color){ 22, 28, 44, 255 }, (Color){ 241, 196, 15, 255 }, 0.06f);

    // Modal Header
    const char* modalTitle = Localization::IsKorean() ? "★ 룬 각인 대상 스킬 선택 ★" : "* SELECT TARGET SKILL TO SOCKET RUNE *";
    int mtw = FontManager::MeasureText(modalTitle, FontSize::TITLE_MEDIUM);
    FontManager::DrawText(modalTitle, (int)(modalRec.x + (modalRec.width - mtw) * 0.5f), (int)modalRec.y + 40, FontSize::TITLE_MEDIUM, (Color){ 241, 196, 15, 255 });

    // Chosen Rune Banner
    Rectangle runeBanner = { modalRec.x + 60, modalRec.y + 115, modalRec.width - 120, 95 };
    DrawRectangleRounded(runeBanner, 0.10f, 6, (Color){ 32, 40, 60, 240 });
    DrawRectangleRoundedLinesEx(runeBanner, 0.10f, 6, 2.0f, ToRaylibColor(chosenRune->runeColor));

    std::string bannerText = (Localization::IsKorean() ? "각인할 룬: [" : "Rune to Socket: [") + chosenRune->GetName() + "] (" + std::string(chosenRune->GetCategoryName()) + ")";
    FontManager::DrawText(bannerText.c_str(), (int)runeBanner.x + 25, (int)runeBanner.y + 16, FontSize::CARD_TITLE, ToRaylibColor(chosenRune->runeColor));
    FontManager::DrawText(chosenRune->GetDescription().c_str(), (int)runeBanner.x + 25, (int)runeBanner.y + 54, FontSize::BODY_REGULAR, (Color){ 220, 230, 245, 255 });

    // Instruction Subtitle
    const char* prompt = Localization::IsKorean() ? 
        "아래 4개 스킬 중 룬을 각인할 스킬 카드를 클릭하세요. (스킬당 최대 2개 룬 장착 가능)" :
        "Click one of your 4 skills below to socket this mutation rune. (Max 2 runes per skill)";
    int ptw = FontManager::MeasureText(prompt, FontSize::BODY_REGULAR);
    FontManager::DrawText(prompt, (int)(modalRec.x + (modalRec.width - ptw) * 0.5f), (int)modalRec.y + 230, FontSize::BODY_REGULAR, (Color){ 190, 200, 220, 255 });

    // 4 Skill Cards Display
    const auto& skills = skillSystem.GetSkills();
    float skillCardW = 370.0f;
    float skillCardH = 460.0f;
    float skillSpacing = 25.0f;
    float startX = modalRec.x + 60.0f;
    float startY = modalRec.y + 275.0f;

    for (size_t i = 0; i < skills.size() && i < 4; ++i) {
        const Skill& skill = skills[i];
        Rectangle scRec = { startX + (float)i * (skillCardW + skillSpacing), startY, skillCardW, skillCardH };

        bool isHovered = CheckCollisionPointRec(mousePos, scRec);
        Color borderCol = isHovered ? (Color){ 241, 196, 15, 255 } : ToRaylibColor(skill.GetEffectiveThemeColor());
        Color bgCol = isHovered ? (Color){ 35, 45, 68, 255 } : (Color){ 25, 32, 50, 240 };

        CombatRenderer::DrawCard(scRec, bgCol, borderCol, 0.08f);

        // Hotkey & Skill Name
        std::string sName = "[" + std::to_string(i + 1) + "] " + skill.GetName();
        if (Localization::IsKorean()) {
            if (skill.GetId() == "skill_water") sName = "[1] 급류 베기 (Torrent)";
            else if (skill.GetId() == "skill_fire") sName = "[2] 발화 플라스크 (Ignite)";
            else if (skill.GetId() == "skill_lightning") sName = "[3] 낙뢰 강타 (Thunder)";
            else if (skill.GetId() == "skill_ice") sName = "[4] 빙하의 창 (Glacial)";
        }
        FontManager::DrawText(sName.c_str(), (int)scRec.x + 18, (int)scRec.y + 18, FontSize::BUTTON_MEDIUM, borderCol);

        // Current Effective Element
        Element curElem = skill.GetFinalElement();
        std::string elemStr = Localization::IsKorean() ? ("속성: " + std::string(Localization::GetElementName(curElem))) : ("Element: " + std::string(Localization::GetElementName(curElem)));
        FontManager::DrawText(elemStr.c_str(), (int)scRec.x + 18, (int)scRec.y + 54, FontSize::BODY_SMALL, ToRaylibColor(GetElementColorRGBA(curElem)));

        // Stats Preview
        std::string dmgStr = Localization::IsKorean() ? ("기본 피해량: " + std::to_string(skill.GetFinalDamage())) : ("Damage: " + std::to_string(skill.GetFinalDamage()));
        FontManager::DrawText(dmgStr.c_str(), (int)scRec.x + 18, (int)scRec.y + 88, FontSize::BODY_REGULAR, (Color){ 241, 196, 15, 255 });

        std::string cdStr = Localization::IsKorean() ? ("쿨다운: " + std::to_string(skill.GetFinalCooldown()) + "턴") : ("Cooldown: " + std::to_string(skill.GetFinalCooldown()) + "T");
        FontManager::DrawText(cdStr.c_str(), (int)scRec.x + 18, (int)scRec.y + 120, FontSize::BODY_SMALL, (Color){ 160, 175, 200, 255 });

        // Currently Attached Runes List
        Rectangle socketBox = { scRec.x + 15, scRec.y + 160, scRec.width - 30, 210 };
        DrawRectangleRounded(socketBox, 0.08f, 6, (Color){ 16, 20, 32, 230 });
        DrawRectangleRoundedLinesEx(socketBox, 0.08f, 6, 1.5f, (Color){ 55, 65, 85, 255 });

        FontManager::DrawText(Localization::IsKorean() ? "장착된 룬 목록 (최대 2개):" : "Socketed Runes (Max 2):", (int)socketBox.x + 12, (int)socketBox.y + 12, FontSize::BODY_SMALL, (Color){ 160, 175, 200, 255 });

        const auto& runes = skill.GetSocketedRunes();
        if (runes.empty()) {
            FontManager::DrawText(Localization::IsKorean() ? "(장착된 룬 없음 - 빈 슬롯)" : "(Empty - No Runes Socketed)", (int)socketBox.x + 12, (int)socketBox.y + 45, FontSize::CAPTION, (Color){ 120, 130, 150, 255 });
        } else {
            float ry = socketBox.y + 42;
            for (size_t r = 0; r < runes.size(); ++r) {
                std::string rLabel = "★ " + runes[r].GetName();
                FontManager::DrawText(rLabel.c_str(), (int)socketBox.x + 12, (int)ry, FontSize::BODY_SMALL, ToRaylibColor(runes[r].runeColor));
                ry += 32;
            }
        }

        // Socket Button
        Rectangle socketBtn = { scRec.x + 20, scRec.y + scRec.height - 65, scRec.width - 40, 48 };
        const char* sBtnLabel = Localization::IsKorean() ? "이 스킬에 각인" : "Socket Rune";
        CombatRenderer::DrawButton(socketBtn, sBtnLabel, (Color){ 39, 174, 96, 220 }, (Color){ 46, 204, 113, 255 }, false, false, FontSize::BUTTON_MEDIUM);
    }

    // Cancel Button at bottom of modal
    Rectangle cancelRec = { modalRec.x + modalRec.width * 0.5f - 180.0f, modalRec.y + modalRec.height - 70.0f, 360.0f, 50.0f };
    const char* cancelLabel = Localization::IsKorean() ? "선택 취소 (다른 룬 고르기)" : "Cancel (Pick Another Rune)";
    CombatRenderer::DrawButton(cancelRec, cancelLabel, (Color){ 108, 92, 231, 220 }, (Color){ 155, 89, 182, 255 }, false, false, FontSize::BUTTON_MEDIUM);
}

void RewardRenderer::DrawRewardScreen(const RewardSystem& rewardSystem, const SkillSystem& skillSystem) {
    int w = ScreenConfig::VIRTUAL_WIDTH;
    int h = ScreenConfig::VIRTUAL_HEIGHT;
    Vector2 mousePos = ScreenConfig::GetVirtualMousePosition();

    // Background Gradient Overlay
    DrawRectangleGradientV(0, 0, w, h, (Color){ 10, 15, 25, 240 }, (Color){ 20, 26, 42, 250 });

    // Main Header Panel
    Rectangle headerRec = { (float)w * 0.5f - 750.0f, 70.0f, 1500.0f, 140.0f };
    CombatRenderer::DrawCard(headerRec, (Color){ 20, 26, 40, 245 }, (Color){ 241, 196, 15, 255 }, 0.08f);

    const char* title = Localization::IsKorean() ? "★ 웨이브 클리어 보상: 변이 룬 선택 ★" : "* WAVE CLEARED - MUTATION RUNE REWARD *";
    int tw = FontManager::MeasureText(title, FontSize::TITLE_MEDIUM);
    FontManager::DrawText(title, (int)(headerRec.x + (headerRec.width - tw) * 0.5f), (int)headerRec.y + 25, FontSize::TITLE_MEDIUM, (Color){ 241, 196, 15, 255 });

    const char* subtitle = Localization::IsKorean() ? 
        "아래 3개의 무작위 변이 룬 중 하나를 선택하여 기존 스킬에 각인하세요!" :
        "Choose one of the 3 Mutation Runes below to empower and mutate your skills!";
    int stw = FontManager::MeasureText(subtitle, FontSize::CARD_TITLE);
    FontManager::DrawText(subtitle, (int)(headerRec.x + (headerRec.width - stw) * 0.5f), (int)headerRec.y + 85, FontSize::CARD_TITLE, (Color){ 190, 205, 225, 255 });

    // 3 Rune Cards
    const auto& offeredRunes = rewardSystem.GetOfferedRunes();
    float cardW = 680.0f;
    float cardH = 960.0f;
    float spacing = 50.0f;
    float totalW = 3 * cardW + 2 * spacing;
    float startX = ((float)w - totalW) * 0.5f;
    float startY = 260.0f;

    int selectedIdx = rewardSystem.GetSelectedRuneIndex();

    for (size_t i = 0; i < offeredRunes.size() && i < 3; ++i) {
        Rectangle cardRec = { startX + (float)i * (cardW + spacing), startY, cardW, cardH };
        bool isHovered = CheckCollisionPointRec(mousePos, cardRec);
        bool isSelected = (static_cast<int>(i) == selectedIdx);

        DrawRuneCard(cardRec, offeredRunes[i], (int)i, isHovered, isSelected);
    }

    // If a rune was clicked, draw the Socketing Modal on top
    if (rewardSystem.IsSocketingPopupOpen()) {
        DrawSocketingModal(rewardSystem, skillSystem);
    }
}
