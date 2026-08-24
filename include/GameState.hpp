#pragma once

#include "Core/Constants.hpp"
#include "Systems/CombatSystem.hpp"
#include "Systems/RewardSystem.hpp"
#include "Systems/MapSystem.hpp"
#include "Renderer/ParticleSystem.hpp"
#include "Renderer/UIRenderer.hpp"
#include "Renderer/RewardRenderer.hpp"
#include "Renderer/MapRenderer.hpp"

class GameManager {
private:
    AppState state;
    ParticleSystem particleSystem;
    CombatSystem combatSystem;
    RewardSystem rewardSystem;
    MapSystem mapSystem;
    UIRenderer uiRenderer;
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

    RewardSystem& GetRewardSystem() { return rewardSystem; }
    const RewardSystem& GetRewardSystem() const { return rewardSystem; }

    MapSystem& GetMapSystem() { return mapSystem; }
    const MapSystem& GetMapSystem() const { return mapSystem; }
};
