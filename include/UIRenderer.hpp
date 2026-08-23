#pragma once

#include "Common.hpp"
#include "CombatSystem.hpp"

class UIRenderer {
private:
    Font customFont;
    bool fontLoaded;
    bool showCheatSheet;

public:
    UIRenderer();
    ~UIRenderer();

    void Initialize();
    void DrawCombatScreen(CombatSystem& combat);
    void DrawTitleScreen();
    void DrawVictoryScreen(const CombatSystem& combat);
    void DrawDefeatScreen(const CombatSystem& combat);
    void DrawSynergyGuideModal();

    bool IsCheatSheetVisible() const { return showCheatSheet; }
    void ToggleCheatSheet() { showCheatSheet = !showCheatSheet; }

private:
    void DrawBackground(WeatherType weather);
    void DrawWeatherForecast(const WeatherSystem& weatherSystem);
    void DrawPlayerCard(const Player& player, StanceType selectedStance);
    void DrawEnemyCards(CombatSystem& combat);
    void DrawStanceSelector(CombatSystem& combat);
    void DrawSkillCards(CombatSystem& combat);
    void DrawExecuteButton(CombatSystem& combat);
    void DrawCombatLog(const std::vector<CombatLogEntry>& log);
    void DrawStatusBadges(const std::vector<StatusInstance>& statuses, Vector2 startPos);
    void DrawHealthBar(Vector2 pos, Vector2 size, int currentHp, int maxHp, int shield, Color fillColor);
    
    // UI Helpers
    void DrawCustomCard(Rectangle rec, Color bg, Color border, float roundness = 0.1f);
    bool DrawButton(Rectangle rec, const char* text, Color baseColor, Color hoverColor, bool active = false, bool disabled = false);
};
