#pragma once

#include "Core/Constants.hpp"
#include "Systems/CombatSystem.hpp"

class CombatRenderer {
public:
    static void DrawBackground(WeatherType weather);
    static void DrawWeatherForecast(const WeatherSystem& weatherSystem);
    static void DrawPlayerPanel(const Player& player, StanceType selectedStance);
    static void DrawEnemyPanel(const CombatSystem& combat);
    static void DrawStancePanel(const CombatSystem& combat);
    static void DrawSkillPanel(const CombatSystem& combat);
    static void DrawExecuteButton(const CombatSystem& combat);
    static void DrawLogPanel(const std::vector<CombatLogEntry>& log);

    // Common Drawing Helpers
    static void DrawCard(Rectangle rec, Color bg, Color border, float roundness = 0.08f);
    static bool DrawButton(Rectangle rec, const char* text, Color baseColor, Color hoverColor,
                           bool active = false, bool disabled = false, int fontSize = 24);
    static void DrawHealthBar(Vector2 pos, Vector2 size, int currentHp, int maxHp, int shield, Color fillColor);
    static void DrawStatusBadges(const std::vector<StatusInstance>& statuses, Vector2 startPos);
};
