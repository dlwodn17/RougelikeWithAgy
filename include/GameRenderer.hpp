#pragma once

#include "Common.hpp"
#include "CombatSystem.hpp"
#include "ParticleSystem.hpp"

class GameRenderer {
public:
    GameRenderer(int virtualWidth = 2560, int virtualHeight = 1440);
    ~GameRenderer();

    void init();
    void render(const CombatSystem& combat, const ParticleSystem& particles, GameScene scene, int selectedSettingIdx = 0, bool showGuide = false, bool showSettings = false);

    // Helpers
    static Color getElementColor(Element elem, float alpha = 1.0f);
    static void drawCard(Rectangle rec, Color bg, Color border, float roundness = 0.08f);
    static bool drawButton(Rectangle rec, const char* text, Color baseColor, Color hoverColor, bool active = false, bool disabled = false, int fontSize = 24);
    static void drawHealthBar(Vector2 pos, Vector2 size, int currentHp, int maxHp, int shield, Color fillColor);
    static void drawStatusBadges(const std::vector<StatusInstance>& statuses, Vector2 startPos);

    int getVirtualWidth() const { return m_virtualWidth; }
    int getVirtualHeight() const { return m_virtualHeight; }

    // 2560x1440 High-Res Layout Metrics
    static constexpr float VIRTUAL_W = 2560.0f;
    static constexpr float VIRTUAL_H = 1440.0f;

    static constexpr float TOP_BAR_X = 40.0f;
    static constexpr float TOP_BAR_Y = 20.0f;
    static constexpr float TOP_BAR_W = 2480.0f;
    static constexpr float TOP_BAR_H = 110.0f;

    static constexpr float PLAYER_CARD_X = 60.0f;
    static constexpr float PLAYER_CARD_Y = 160.0f;
    static constexpr float PLAYER_CARD_W = 620.0f;
    static constexpr float PLAYER_CARD_H = 620.0f;

    static constexpr float ENEMY_START_X = 720.0f;
    static constexpr float ENEMY_CARD_Y = 160.0f;
    static constexpr float ENEMY_CARD_H = 620.0f;

    static constexpr float STANCE_PANEL_X = 60.0f;
    static constexpr float STANCE_PANEL_Y = 810.0f;
    static constexpr float STANCE_PANEL_W = 620.0f;
    static constexpr float STANCE_PANEL_H = 260.0f;

    static constexpr float SKILL_TRAY_X = 720.0f;
    static constexpr float SKILL_TRAY_Y = 810.0f;
    static constexpr float SKILL_CARD_W = 330.0f;
    static constexpr float SKILL_CARD_H = 260.0f;

    static constexpr float EXECUTE_BTN_X = 2160.0f;
    static constexpr float EXECUTE_BTN_Y = 810.0f;
    static constexpr float EXECUTE_BTN_W = 330.0f;
    static constexpr float EXECUTE_BTN_H = 260.0f;

    static constexpr float LOG_PANEL_X = 60.0f;
    static constexpr float LOG_PANEL_Y = 1100.0f;
    static constexpr float LOG_PANEL_W = 2440.0f;
    static constexpr float LOG_PANEL_H = 290.0f;

private:
    void drawBackground(WeatherType weather) const;
    void drawWeatherForecast(const WeatherSystem& weather) const;
    void drawPlayerPanel(const Player& player, StanceType stance) const;
    void drawEnemyPanel(const CombatSystem& combat) const;
    void drawStancePanel(const CombatSystem& combat) const;
    void drawSkillPanel(const CombatSystem& combat) const;
    void drawExecuteButton(const CombatSystem& combat) const;
    void drawLogPanel(const std::vector<CombatLogEntry>& log) const;

    void drawStartScreen() const;
    void drawVictoryOverlay(const CombatSystem& combat) const;
    void drawGameOverOverlay(const CombatSystem& combat) const;
    void drawSettingsOverlay(int selectedIdx) const;
    void drawGuideOverlay() const;

    int m_virtualWidth;
    int m_virtualHeight;
};

// Aliases for compatibility
using UIRenderer = GameRenderer;
