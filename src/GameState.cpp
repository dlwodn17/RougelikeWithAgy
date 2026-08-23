#include "GameState.hpp"

GameManager::GameManager() : state(AppState::TITLE) {
}

void GameManager::Initialize() {
    uiRenderer.Initialize();
    combatSystem.InitializeNewRun();
}

void GameManager::Update(float dt) {
    switch (state) {
        case AppState::TITLE:
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                combatSystem.InitializeNewRun();
                state = AppState::BATTLE;
            }
            break;

        case AppState::BATTLE:
            combatSystem.Update(dt);

            // Check if CombatSystem entered victory or defeat
            if (combatSystem.GetPhase() == CombatPhase::VICTORY_SCREEN) {
                state = AppState::VICTORY;
            } else if (combatSystem.GetPhase() == CombatPhase::DEFEAT_SCREEN) {
                state = AppState::GAME_OVER;
            }

            // Quick restart hotkey
            if (IsKeyPressed(KEY_R)) {
                combatSystem.RestartGame();
            }
            break;

        case AppState::VICTORY:
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                if (combatSystem.GetCurrentWave() >= combatSystem.GetMaxWaves()) {
                    combatSystem.RestartGame();
                } else {
                    combatSystem.NextWave();
                }
                state = AppState::BATTLE;
            }
            break;

        case AppState::GAME_OVER:
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
                combatSystem.RestartGame();
                state = AppState::BATTLE;
            }
            break;

        default:
            break;
    }
}

void GameManager::Draw() {
    switch (state) {
        case AppState::TITLE:
            uiRenderer.DrawTitleScreen();
            break;

        case AppState::BATTLE:
            uiRenderer.DrawCombatScreen(combatSystem);
            break;

        case AppState::VICTORY:
            uiRenderer.DrawCombatScreen(combatSystem);
            uiRenderer.DrawVictoryScreen(combatSystem);
            break;

        case AppState::GAME_OVER:
            uiRenderer.DrawCombatScreen(combatSystem);
            uiRenderer.DrawDefeatScreen(combatSystem);
            break;

        default:
            break;
    }
}
