#pragma once

#include "Common.hpp"
#include "Entity.hpp"
#include "WeatherSystem.hpp"
#include "SkillSystem.hpp"
#include "ElementalSystem.hpp"
#include "ParticleSystem.hpp"

class CombatSystem {
private:
    Player player;
    std::vector<Enemy> enemies;
    WeatherSystem weatherSystem;
    ParticleSystem particleSystem;

    CombatPhase currentPhase;
    float phaseTimer;
    int currentEnemyActionIndex;

    int selectedSkillIndex;
    int selectedTargetIndex;
    StanceType selectedStance;

    int currentWave;
    int maxWaves;
    int turnCounter;

    std::vector<CombatLogEntry> combatLog;

public:
    CombatSystem();

    void InitializeNewRun();
    void StartWave(int waveNumber);
    void Update(float dt);

    // Player Input Handlers
    void SelectSkill(int index);
    void SelectTarget(int index);
    void SelectStance(StanceType stance);
    bool ExecutePlayerTurn();

    // Deterministic 6-Step Turn Resolution Pipeline
    void Step1_ResolvePlayerAction();
    void Step2_ResolveEnemyActionStep();
    void Step3_TickStatusEffects();
    void Step4_TickCooldowns();
    void Step5_AdvanceWeatherAndApply();
    void Step6_ResetTurnAndStartNext();

    // Progression
    void NextWave();
    void RestartGame();

    // Getters
    Player& GetPlayer() { return player; }
    const Player& GetPlayer() const { return player; }

    std::vector<Enemy>& GetEnemies() { return enemies; }
    const std::vector<Enemy>& GetEnemies() const { return enemies; }

    WeatherSystem& GetWeatherSystem() { return weatherSystem; }
    const WeatherSystem& GetWeatherSystem() const { return weatherSystem; }

    SkillSystem& GetSkillSystem() { return player.GetSkillSystem(); }
    const SkillSystem& GetSkillSystem() const { return player.GetSkillSystem(); }

    ParticleSystem& GetParticleSystem() { return particleSystem; }

    CombatPhase GetPhase() const { return currentPhase; }
    int GetSelectedSkillIndex() const { return selectedSkillIndex; }
    int GetSelectedTargetIndex() const { return selectedTargetIndex; }
    StanceType GetSelectedStance() const { return selectedStance; }
    int GetCurrentWave() const { return currentWave; }
    int GetMaxWaves() const { return maxWaves; }
    int GetTurnCounter() const { return turnCounter; }

    const std::vector<CombatLogEntry>& GetCombatLog() const { return combatLog; }
    void AddCombatLog(const std::string& text, Color color = WHITE);

    void CheckBattleEndConditions();
};
