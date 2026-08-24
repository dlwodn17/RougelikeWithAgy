#include "Renderer/MapRenderer.hpp"
#include "Renderer/FontManager.hpp"
#include "Renderer/CombatRenderer.hpp"
#include "Core/Localization.hpp"
#include <cmath>

void MapRenderer::DrawPathLine(Vector2 start, Vector2 end, bool isEroded, bool isAvailable, bool isVisited) {
    Color lineColor;
    float thickness = 3.0f;

    if (isEroded) {
        lineColor = (Color){ 75, 35, 95, 140 };
        thickness = 2.0f;
    } else if (isAvailable) {
        lineColor = (Color){ 241, 196, 15, 230 };
        thickness = 4.5f;
    } else if (isVisited) {
        lineColor = (Color){ 46, 204, 113, 200 };
        thickness = 3.5f;
    } else {
        lineColor = (Color){ 90, 110, 140, 160 };
        thickness = 2.5f;
    }

    DrawLineEx(start, end, thickness, lineColor);
}

void MapRenderer::DrawNode(const MapNode& node, bool isHovered) {
    Vector2 center = { node.posX, node.posY };
    float r = node.radius * (isHovered ? 1.18f : 1.0f);

    Color themeCol = ToRaylibColor(GetNodeColorRGBA(node.type));
    Color bgCol = (Color){ 20, 26, 40, 240 };
    Color borderCol = themeCol;

    if (node.isEroded) {
        bgCol = (Color){ 25, 15, 30, 220 };
        borderCol = (Color){ 95, 45, 110, 180 };
        themeCol = (Color){ 120, 80, 140, 180 };
    } else if (node.isCurrent) {
        bgCol = (Color){ 35, 55, 85, 255 };
        borderCol = (Color){ 241, 196, 15, 255 };
    } else if (node.isAvailable) {
        bgCol = isHovered ? (Color){ 45, 60, 90, 255 } : (Color){ 30, 40, 60, 245 };
        borderCol = isHovered ? WHITE : (Color){ 241, 196, 15, 255 };
    } else if (node.isVisited) {
        bgCol = (Color){ 22, 35, 30, 230 };
        borderCol = (Color){ 46, 204, 113, 200 };
    }

    // 1. Pulsing Halo for available nodes
    if (node.isAvailable && !node.isEroded) {
        float pulse = 0.5f + 0.5f * std::sin(node.pulseTimer);
        float haloR = r + 6.0f + pulse * 8.0f;
        DrawCircleV(center, haloR, (Color){ 241, 196, 15, (unsigned char)(60 + pulse * 70) });
    }

    // 2. Current player location indicator ring
    if (node.isCurrent) {
        float ringR = r + 10.0f;
        DrawCircleLines((int)center.x, (int)center.y, ringR, (Color){ 52, 152, 219, 255 });
    }

    // 3. Node Main Body Circle
    DrawCircleV(center, r, bgCol);
    DrawCircleLines((int)center.x, (int)center.y, r, borderCol);
    DrawCircleLines((int)center.x, (int)center.y, r - 2.5f, borderCol);

    // 4. Node Icon / Symbol
    const char* symbol = GetNodeSymbol(node.type);
    if (node.isVisited) symbol = "✓";
    else if (node.isEroded) symbol = "✕";

    int fs = (node.type == NodeType::BOSS) ? FontSize::TITLE_MEDIUM : FontSize::BUTTON_LARGE;
    int tw = FontManager::MeasureText(symbol, fs);
    FontManager::DrawText(symbol, (int)(center.x - (float)tw * 0.5f), (int)(center.y - (float)fs * 0.5f), fs, themeCol);

    // 5. Node Name underneath
    std::string nameLabel = Localization::IsKorean() ? GetNodeTypeNameKo(node.type) : GetNodeTypeNameEn(node.type);
    if (node.erosionRisk > 0 && !node.isEroded && node.type != NodeType::BOSS) {
        nameLabel += Localization::IsKorean() ? " [위험]" : " [Surge]";
    }
    int ntw = FontManager::MeasureText(nameLabel.c_str(), FontSize::CAPTION);
    Color labelCol = node.isEroded ? (Color){ 140, 100, 160, 180 } : (node.isAvailable ? WHITE : (Color){ 170, 185, 205, 220 });
    FontManager::DrawText(nameLabel.c_str(), (int)(center.x - (float)ntw * 0.5f), (int)(center.y + r + 8.0f), FontSize::CAPTION, labelCol);
}

void MapRenderer::DrawNodeTooltip(const MapNode& node, Vector2 mousePos) {
    float tipW = 380.0f;
    float tipH = 170.0f;
    float tipX = mousePos.x + 25.0f;
    float tipY = mousePos.y + 20.0f;

    if (tipX + tipW > (float)ScreenConfig::VIRTUAL_WIDTH - 20.0f) {
        tipX = mousePos.x - tipW - 25.0f;
    }
    if (tipY + tipH > (float)ScreenConfig::VIRTUAL_HEIGHT - 20.0f) {
        tipY = mousePos.y - tipH - 20.0f;
    }

    Rectangle tipRec = { tipX, tipY, tipW, tipH };
    DrawRectangleRounded(tipRec, 0.12f, 6, (Color){ 18, 24, 38, 245 });
    DrawRectangleRoundedLinesEx(tipRec, 0.12f, 6, 2.0f, ToRaylibColor(GetNodeColorRGBA(node.type)));

    std::string title = (Localization::IsKorean() ? GetNodeTypeNameKo(node.type) : GetNodeTypeNameEn(node.type));
    title += " (Floor " + std::to_string(node.layer + 1) + ")";
    FontManager::DrawText(title.c_str(), (int)tipX + 16, (int)tipY + 14, FontSize::BODY_REGULAR, ToRaylibColor(GetNodeColorRGBA(node.type)));

    std::string desc = "";
    if (node.type == NodeType::COMBAT) {
        desc = Localization::IsKorean() ? "원소 마법사들과 슬라임 무리를 격퇴하고 변이 룬을 획득합니다." : "Defeat elemental minions and claim mutation runes.";
    } else if (node.type == NodeType::ELITE) {
        desc = Localization::IsKorean() ? "강력한 엘리트 수호자와 결투를 벌이고 희귀 룬 보상을 확정 획득합니다." : "Face a deadly elite guardian for guaranteed rare rune rewards.";
    } else if (node.type == NodeType::REST_SITE) {
        desc = Localization::IsKorean() ? "모닥불에서 체력을 35% 회복하거나 룬 슬롯을 정화/교체합니다." : "Rest by the hearth: Heal 35% HP or cleanse socketed runes.";
    } else if (node.type == NodeType::WEATHER_SHRINE) {
        desc = Localization::IsKorean() ? "원소의 제단에서 앞으로의 날씨 예보를 원하는 속성으로 재구성합니다." : "Commune with the shrine to manipulate future weather forecasts.";
    } else if (node.type == NodeType::BOSS) {
        desc = Localization::IsKorean() ? "원소 수렴의 탑 최종 보스 [원소의 아콘]과 결전을 벌입니다." : "The pinnacle of the tower. Slay the Elemental Archon!";
    }

    FontManager::DrawText(desc.c_str(), (int)tipX + 16, (int)tipY + 48, FontSize::CAPTION, (Color){ 200, 215, 235, 255 });

    const char* prompt = Localization::IsKorean() ? "▶ 마우스 클릭하여 이 방으로 이동" : "▶ Click to enter this node";
    FontManager::DrawText(prompt, (int)tipX + 16, (int)tipY + 130, FontSize::CAPTION, (Color){ 241, 196, 15, 255 });
}

void MapRenderer::DrawMapScreen(const MapSystem& mapSystem, const Player& player, const WeatherSystem& weatherSystem) {
    int w = ScreenConfig::VIRTUAL_WIDTH;
    int h = ScreenConfig::VIRTUAL_HEIGHT;
    Vector2 mousePos = ScreenConfig::GetVirtualMousePosition();

    // 1. Deep Celestial Background
    DrawRectangleGradientV(0, 0, w, h, (Color){ 12, 16, 28, 255 }, (Color){ 18, 22, 38, 255 });

    // 2. Erosion Abyss Shading (Past layers in the fog of the void)
    int currentLayer = mapSystem.GetCurrentLayer();
    if (currentLayer > 0) {
        float startX = 320.0f;
        float endX = 2260.0f;
        float layerSpacingX = (endX - startX) / static_cast<float>(MapSystem::TOTAL_LAYERS - 1);
        float erodedBoundX = startX + static_cast<float>(currentLayer - 1) * layerSpacingX + layerSpacingX * 0.5f;

        Rectangle abyssRec = { 0, 180.0f, erodedBoundX, (float)h - 260.0f };
        DrawRectangleGradientH((int)abyssRec.x, (int)abyssRec.y, (int)abyssRec.width, (int)abyssRec.height,
                               (Color){ 35, 10, 30, 200 }, (Color){ 20, 12, 28, 100 });
        DrawRectangleRoundedLinesEx(abyssRec, 0.02f, 4, 1.5f, (Color){ 120, 40, 90, 160 });

        const char* erosionTag = Localization::IsKorean() ? "◀ [침식된 하위 층 - 붕괴 완료]" : "◀ [Eroded Sub-Floors - Collapsed into Void]";
        FontManager::DrawText(erosionTag, 80, 210, FontSize::BODY_SMALL, (Color){ 180, 80, 130, 220 });
    }

    // 3. Top Header Bar
    Rectangle headerRec = { 60.0f, 30.0f, (float)w - 120.0f, 130.0f };
    CombatRenderer::DrawCard(headerRec, (Color){ 22, 28, 44, 240 }, (Color){ 241, 196, 15, 255 }, 0.08f);

    const char* mapTitle = Localization::IsKorean() ? "★ 원소 수렴의 탑 - 층간 경로 선택 (Tower of Convergence) ★" : "* TOWER OF ELEMENTAL CONVERGENCE - FLOOR MAP *";
    FontManager::DrawText(mapTitle, (int)headerRec.x + 30, (int)headerRec.y + 20, FontSize::TITLE_MEDIUM - 2, (Color){ 241, 196, 15, 255 });

    // Header Vitals: Player HP, Layer depth, and Weather info
    std::string hpText = (Localization::IsKorean() ? "영웅 체력: " : "Hero HP: ") + std::to_string(player.GetHp()) + " / " + std::to_string(player.GetMaxHp());
    FontManager::DrawText(hpText.c_str(), (int)headerRec.x + 30, (int)headerRec.y + 76, FontSize::BODY_REGULAR, (Color){ 46, 204, 113, 255 });

    std::string layerText = (Localization::IsKorean() ? "진행 층: 제 " : "Current Depth: Floor ") + std::to_string(std::max(1, currentLayer + 1)) + " / 8 층";
    FontManager::DrawText(layerText.c_str(), (int)headerRec.x + 460, (int)headerRec.y + 76, FontSize::BODY_REGULAR, (Color){ 100, 200, 255, 255 });

    Element curWeatherElem = weatherSystem.GetWeatherSynergyElement();
    std::string weatherText = (Localization::IsKorean() ? "활성 날씨: [" : "Active Weather: [") + std::string(Localization::GetWeatherName(weatherSystem.GetCurrentWeather())) + "]";
    FontManager::DrawText(weatherText.c_str(), (int)headerRec.x + 880, (int)headerRec.y + 76, FontSize::BODY_REGULAR, ToRaylibColor(GetElementColorRGBA(curWeatherElem)));

    std::string promptText = Localization::IsKorean() ? "황금빛으로 빛나는 연결된 다음 노드를 클릭하여 전진하세요!" : "Click one of the glowing available nodes to advance!";
    FontManager::DrawText(promptText.c_str(), (int)headerRec.x + 1400, (int)headerRec.y + 76, FontSize::BODY_REGULAR, (Color){ 241, 196, 15, 255 });

    // 4. Draw Graph Path Lines
    const auto& nodes = mapSystem.GetAllNodes();
    for (const auto& node : nodes) {
        Vector2 startPos = { node.posX, node.posY };
        for (int nextId : node.nextNodeIds) {
            const MapNode* nextNode = mapSystem.GetNode(nextId);
            if (nextNode) {
                Vector2 endPos = { nextNode->posX, nextNode->posY };
                bool isEroded = node.isEroded || nextNode->isEroded;
                bool isAvailable = (node.isCurrent && nextNode->isAvailable);
                bool isVisited = (node.isVisited && nextNode->isVisited);
                DrawPathLine(startPos, endPos, isEroded, isAvailable, isVisited);
            }
        }
    }

    // 5. Draw Nodes
    const MapNode* hoveredNode = nullptr;
    for (const auto& node : nodes) {
        Vector2 center = { node.posX, node.posY };
        bool isHovered = CheckCollisionPointCircle(mousePos, center, node.radius) && node.isAvailable;
        DrawNode(node, isHovered);

        if (isHovered) {
            hoveredNode = &node;
        }
    }

    // 6. Draw Bottom Legend Bar
    Rectangle legendRec = { 60.0f, (float)h - 95.0f, (float)w - 120.0f, 65.0f };
    DrawRectangleRounded(legendRec, 0.12f, 6, (Color){ 16, 22, 34, 230 });
    DrawRectangleRoundedLinesEx(legendRec, 0.12f, 6, 1.2f, (Color){ 65, 75, 95, 255 });

    const char* legendText = Localization::IsKorean() ?
        "[⚔ 일반 전투]     [☠ 엘리트 전투 (희귀 룬)]     [⛺ 휴식처 (체력 35% 회복/정화)]     [⚡ 날씨 제단 (예보 조작)]     [★ 탑의 최종 보스]" :
        "[⚔ Combat]     [☠ Elite Battle (Rare Runes)]     [⛺ Rest Sanctuary (Heal 35%)]     [⚡ Weather Shrine (Forecast)]     [★ Final Boss]";
    int ltw = FontManager::MeasureText(legendText, FontSize::BODY_SMALL);
    FontManager::DrawText(legendText, (int)(legendRec.x + (legendRec.width - ltw) * 0.5f), (int)legendRec.y + 20, FontSize::BODY_SMALL, (Color){ 200, 215, 235, 255 });

    // 7. Draw Tooltip if hovering available node
    if (hoveredNode) {
        DrawNodeTooltip(*hoveredNode, mousePos);
    }
}

void MapRenderer::DrawRestScreen(const Player& player, const SkillSystem& /*skillSystem*/) {
    int w = ScreenConfig::VIRTUAL_WIDTH;
    int h = ScreenConfig::VIRTUAL_HEIGHT;
    Vector2 mousePos = ScreenConfig::GetVirtualMousePosition();

    // Background Warm Dark Slate
    DrawRectangleGradientV(0, 0, w, h, (Color){ 15, 20, 28, 250 }, (Color){ 28, 20, 22, 250 });

    // Header Card
    Rectangle headerRec = { (float)w * 0.5f - 750.0f, 100.0f, 1500.0f, 150.0f };
    CombatRenderer::DrawCard(headerRec, (Color){ 24, 30, 42, 245 }, (Color){ 46, 204, 113, 255 }, 0.08f);

    const char* title = Localization::IsKorean() ? "★ 모닥불 휴식처 (Rest Sanctuary) ★" : "* HEARTH REST SANCTUARY *";
    int tw = FontManager::MeasureText(title, FontSize::TITLE_MEDIUM);
    FontManager::DrawText(title, (int)(headerRec.x + (headerRec.width - tw) * 0.5f), (int)headerRec.y + 25, FontSize::TITLE_MEDIUM, (Color){ 46, 204, 113, 255 });

    const char* subtitle = Localization::IsKorean() ?
        "원소의 폭풍을 피해 잠시 안식을 취합니다. 휴식 방식을 하나 선택하세요." :
        "Shelter from the harsh elemental storm. Choose an action to prepare for the ascent.";
    int stw = FontManager::MeasureText(subtitle, FontSize::CARD_TITLE);
    FontManager::DrawText(subtitle, (int)(headerRec.x + (headerRec.width - stw) * 0.5f), (int)headerRec.y + 90, FontSize::CARD_TITLE, (Color){ 200, 215, 230, 255 });

    // 2 Option Cards Side-by-Side
    float cardW = 660.0f;
    float cardH = 680.0f;
    float startX = ((float)w - (2.0f * cardW + 80.0f)) * 0.5f;
    float startY = 320.0f;

    // Option 1: Deep Rest (Heal 35% HP)
    Rectangle card1Rec = { startX, startY, cardW, cardH };
    bool hov1 = CheckCollisionPointRec(mousePos, card1Rec);
    CombatRenderer::DrawCard(card1Rec, hov1 ? (Color){ 30, 45, 40, 255 } : (Color){ 20, 32, 28, 240 }, (Color){ 46, 204, 113, 255 }, 0.08f);

    FontManager::DrawText(Localization::IsKorean() ? "[1] 깊은 휴식 (Deep Rest)" : "[1] Deep Rest", (int)card1Rec.x + 35, (int)card1Rec.y + 35, FontSize::TITLE_MEDIUM, (Color){ 46, 204, 113, 255 });
    
    int healAmt = static_cast<int>(player.GetMaxHp() * 0.35f);
    int nextHp = std::min(player.GetMaxHp(), player.GetHp() + healAmt);
    std::string hpPreview = (Localization::IsKorean() ? "체력 회복: +" : "HP Restored: +") + std::to_string(healAmt) + 
                            " (" + std::to_string(player.GetHp()) + " -> " + std::to_string(nextHp) + " / " + std::to_string(player.GetMaxHp()) + ")";
    FontManager::DrawText(hpPreview.c_str(), (int)card1Rec.x + 35, (int)card1Rec.y + 110, FontSize::CARD_TITLE, (Color){ 241, 196, 15, 255 });

    const char* c1Desc = Localization::IsKorean() ?
        "모닥불 곁에서 따뜻한 온기를 느끼며 부상을 치유합니다.\n최대 체력의 35%를 즉시 회복합니다." :
        "Rest by the soothing campfire flames.\nInstantly restores 35% of your Max HP.";
    FontManager::DrawText(c1Desc, (int)card1Rec.x + 35, (int)card1Rec.y + 180, FontSize::BODY_REGULAR, (Color){ 190, 210, 225, 255 });

    Rectangle btn1 = { card1Rec.x + 40, card1Rec.y + card1Rec.height - 85, card1Rec.width - 80, 55 };
    CombatRenderer::DrawButton(btn1, Localization::IsKorean() ? "체력 회복하고 출발" : "Heal HP & Proceed", (Color){ 39, 174, 96, 220 }, (Color){ 46, 204, 113, 255 }, false, false, FontSize::BUTTON_MEDIUM);

    // Option 2: Meditation & Cool Focus (Reset All Skill Cooldowns)
    Rectangle card2Rec = { startX + cardW + 80.0f, startY, cardW, cardH };
    bool hov2 = CheckCollisionPointRec(mousePos, card2Rec);
    CombatRenderer::DrawCard(card2Rec, hov2 ? (Color){ 35, 42, 60, 255 } : (Color){ 24, 30, 48, 240 }, (Color){ 52, 152, 219, 255 }, 0.08f);

    FontManager::DrawText(Localization::IsKorean() ? "[2] 마력 집중 (Meditation)" : "[2] Meditation", (int)card2Rec.x + 35, (int)card2Rec.y + 35, FontSize::TITLE_MEDIUM, (Color){ 52, 152, 219, 255 });

    FontManager::DrawText(Localization::IsKorean() ? "모든 스킬 쿨다운 즉시 초기화" : "Reset All Skill Cooldowns", (int)card2Rec.x + 35, (int)card2Rec.y + 110, FontSize::CARD_TITLE, (Color){ 241, 196, 15, 255 });

    const char* c2Desc = Localization::IsKorean() ?
        "정신을 맑게 가다듬고 원소 마력을 조율합니다.\n모든 스킬의 쿨다운을 초기화하여 즉시 사용 가능한 상태로 만듭니다." :
        "Focus your mind and realign elemental mana flow.\nResets all skill cooldowns to 0 turns immediately.";
    FontManager::DrawText(c2Desc, (int)card2Rec.x + 35, (int)card2Rec.y + 180, FontSize::BODY_REGULAR, (Color){ 190, 210, 225, 255 });

    Rectangle btn2 = { card2Rec.x + 40, card2Rec.y + card2Rec.height - 85, card2Rec.width - 80, 55 };
    CombatRenderer::DrawButton(btn2, Localization::IsKorean() ? "마력 집중하고 출발" : "Meditate & Proceed", (Color){ 41, 128, 185, 220 }, (Color){ 52, 152, 219, 255 }, false, false, FontSize::BUTTON_MEDIUM);
}

void MapRenderer::DrawShrineScreen(const WeatherSystem& weatherSystem) {
    int w = ScreenConfig::VIRTUAL_WIDTH;
    int h = ScreenConfig::VIRTUAL_HEIGHT;
    Vector2 mousePos = ScreenConfig::GetVirtualMousePosition();

    // Background Gradient
    DrawRectangleGradientV(0, 0, w, h, (Color){ 15, 18, 30, 250 }, (Color){ 22, 32, 48, 250 });

    // Header Card
    Rectangle headerRec = { (float)w * 0.5f - 750.0f, 80.0f, 1500.0f, 140.0f };
    CombatRenderer::DrawCard(headerRec, (Color){ 22, 28, 44, 245 }, (Color){ 52, 152, 219, 255 }, 0.08f);

    const char* title = Localization::IsKorean() ? "★ 날씨의 제단 (Altar of Elements) ★" : "* ALTAR OF THE ELEMENTS *";
    int tw = FontManager::MeasureText(title, FontSize::TITLE_MEDIUM);
    FontManager::DrawText(title, (int)(headerRec.x + (headerRec.width - tw) * 0.5f), (int)headerRec.y + 16, FontSize::TITLE_MEDIUM, (Color){ 52, 152, 219, 255 });

    std::string curWeatherNotice = (Localization::IsKorean() ? "현재 날씨: [" : "Current Weather: [") +
                                   std::string(Localization::GetWeatherName(weatherSystem.GetCurrentWeather())) + 
                                   (Localization::IsKorean() ? "] -> 원하는 원소 날씨를 선택하여 날씨 큐 전체를 변경하세요." : "] -> Choose an elemental attunement to rewrite the entire forecast.");
    int stw = FontManager::MeasureText(curWeatherNotice.c_str(), FontSize::BODY_REGULAR);
    FontManager::DrawText(curWeatherNotice.c_str(), (int)(headerRec.x + (headerRec.width - stw) * 0.5f), (int)headerRec.y + 80, FontSize::BODY_REGULAR, (Color){ 241, 196, 15, 255 });

    // 4 Weather Option Cards
    float cardW = 540.0f;
    float cardH = 820.0f;
    float spacing = 35.0f;
    float totalW = 4.0f * cardW + 3.0f * spacing;
    float startX = ((float)w - totalW) * 0.5f;
    float startY = 260.0f;

    struct ShrineOption {
        WeatherType weather;
        Element elem;
        const char* titleKo;
        const char* titleEn;
        const char* descKo;
        const char* descEn;
        ColorRGBA color;
    };

    ShrineOption options[4] = {
        { WeatherType::RAIN, Element::WET, "[1] 폭우의 축복", "[1] Downpour",
          "대지를 촉촉하게 적시는 폭우를 소환합니다.\n매 턴 적들에게 [수분] 상태를 자동 부여하여\n감전/빙결 콤보를 유발합니다.",
          "Summons endless torrential rain.\nInflicts [WET] status on enemies every turn\nto prime deadly SHOCK and FROZEN combos.",
          { 52, 152, 219, 255 } },

        { WeatherType::HEATWAVE, Element::FIRE, "[2] 폭염의 축복", "[2] Heatwave",
          "작열하는 태양의 폭염을 소환합니다.\n화염 스킬 피해량이 +50% 증가하며\n기름(OIL) 폭발 위력을 극대화합니다.",
          "Summons scorching solar heatwave.\nBoosts Fire DMG by +50% and amplifies\ndevastating EXPLOSION triggers.",
          { 231, 76, 60, 255 } },

        { WeatherType::BLIZZARD, Element::COLD, "[3] 눈보라의 축복", "[3] Blizzard",
          "극저온의 살인적인 눈보라를 소환합니다.\n냉기 스킬 피해량이 +35% 증가하며\n수분 상태 적을 즉시 얼려 턴을 스킵시킵니다.",
          "Summons a sub-zero howling blizzard.\nBoosts Cold DMG by +35% and flash-freezes\nwet targets to skip enemy turns.",
          { 162, 222, 255, 255 } },

        { WeatherType::STORM, Element::LIGHTNING, "[4] 뇌우의 축복", "[4] Thunderstorm",
          "고전압의 번개 폭풍을 소환합니다.\n전기 피해량이 +30% 증가하며\n광역 감전 아크 전파 확률이 비약적으로 상승합니다.",
          "Summons a high-voltage storm.\nBoosts Lightning DMG by +30% and arcs\novercharged shockwaves across all enemies.",
          { 241, 196, 15, 255 } }
    };

    for (int i = 0; i < 4; ++i) {
        Rectangle cardRec = { startX + (float)i * (cardW + spacing), startY, cardW, cardH };
        bool isHovered = CheckCollisionPointRec(mousePos, cardRec);
        Color themeColor = ToRaylibColor(options[i].color);
        Color bgCol = isHovered ? (Color){ 32, 42, 60, 255 } : (Color){ 20, 26, 40, 240 };
        Color borderCol = isHovered ? WHITE : themeColor;

        CombatRenderer::DrawCard(cardRec, bgCol, borderCol, 0.08f);

        const char* cardTitle = Localization::IsKorean() ? options[i].titleKo : options[i].titleEn;
        FontManager::DrawText(cardTitle, (int)cardRec.x + 25, (int)cardRec.y + 30, FontSize::CARD_TITLE, themeColor);

        // Weather Icon & Element Tag Box
        Rectangle badgeBox = { cardRec.x + 20, cardRec.y + 85, cardRec.width - 40, 80 };
        DrawRectangleRounded(badgeBox, 0.12f, 6, (Color){ 14, 18, 28, 220 });
        DrawRectangleRoundedLinesEx(badgeBox, 0.12f, 6, 1.5f, themeColor);

        std::string elemTag = (Localization::IsKorean() ? "원소 친화: " : "Affinity: ") + std::string(Localization::GetElementName(options[i].elem));
        FontManager::DrawText(elemTag.c_str(), (int)badgeBox.x + 20, (int)badgeBox.y + 26, FontSize::BODY_REGULAR, themeColor);

        // Description
        const char* cardDesc = Localization::IsKorean() ? options[i].descKo : options[i].descEn;
        FontManager::DrawText(cardDesc, (int)cardRec.x + 25, (int)cardRec.y + 195, FontSize::BODY_SMALL, (Color){ 200, 215, 235, 255 });

        // Select Button
        Rectangle btnRec = { cardRec.x + 25, cardRec.y + cardRec.height - 75, cardRec.width - 50, 55 };
        const char* btnLabel = Localization::IsKorean() ? "이 날씨 축복 수락" : "Attune Forecast";
        CombatRenderer::DrawButton(btnRec, btnLabel, (Color){ 41, 128, 185, 220 }, (Color){ 52, 152, 219, 255 }, false, false, FontSize::BUTTON_MEDIUM);
    }
}
