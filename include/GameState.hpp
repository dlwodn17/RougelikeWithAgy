#pragma once

#include "Common.hpp"
#include "CombatSystem.hpp"
#include "UIRenderer.hpp"

class GameManager {
private:
    AppState state;
    CombatSystem combatSystem;
    UIRenderer uiRenderer;

public:
    GameManager();

    void Initialize();
    void Update(float dt);
    void Draw();

    AppState GetState() const { return state; }
    void SetState(AppState newState) { state = newState; }
};
