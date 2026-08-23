#pragma once

#include "Common.hpp"
#include "CombatSystem.hpp"
#include "GameRenderer.hpp"

class GameManager {
private:
    AppState state;
    CombatSystem combatSystem;
    GameRenderer renderer;
    bool showSettings;
    bool showGuide;
    int selectedSettingIdx;

public:
    GameManager();

    void Initialize();
    void Update(float dt);
    void Draw();

    AppState GetState() const { return state; }
    void SetState(AppState newState) { state = newState; }

    bool IsSettingsVisible() const { return showSettings; }
    void ToggleSettings() { showSettings = !showSettings; if (showSettings) showGuide = false; }

    bool IsGuideVisible() const { return showGuide; }
    void ToggleGuide() { showGuide = !showGuide; if (showGuide) showSettings = false; }
};
