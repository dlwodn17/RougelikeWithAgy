#pragma once

#include "Core/Constants.hpp"
#include "Entities/Player.hpp"
#include "Entities/Enemy.hpp"
#include "Systems/WeatherSystem.hpp"
#include "Systems/ElementalSystem.hpp"
#include "Renderer/ParticleSystem.hpp"

class CombatSystem {
private:
    Player player;
    std::vector<Enemy> enemies;
    WeatherSystem weatherSystem;
    ParticleSystem* particleSystem;

    CombatPhase currentPhase;
    float phaseTimer;

    int selectedSkillIndex;
    int selectedTargetIndex;
    StanceType selectedStance;

    int currentWave;
    int maxWaves;
    int turnCounter;

    std::vector<CombatLogEntry> combatLog;

public:
    CombatSystem();

    void SetParticleSystem(ParticleSystem* ps) { particleSystem = ps; }

    void InitializeNewRun();
    void StartWave(int waveNumber);
    void Update(float dt);

    // Player Input Handlers
    void SelectSkill(int index);
    void SelectTarget(int index);
    void SelectStance(StanceType stance);
    bool ExecutePlayerTurn();
    void ExecuteTurn();

    // Pipeline Sub-routines
    void ResolvePlayerAction(int skillIdx, int targetIdx, StanceType stance);
    void ResolveEnemyAction(Enemy& enemy, Player& targetPlayer);
    void ApplyActiveWeatherEffect();
    bool CheckWaveCleared();

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

    CombatPhase GetPhase() const { return currentPhase; }
    int GetSelectedSkillIndex() const { return selectedSkillIndex; }
    int GetSelectedTargetIndex() const { return selectedTargetIndex; }
    StanceType GetSelectedStance() const { return selectedStance; }
    int GetCurrentWave() const { return currentWave; }
    int GetMaxWaves() const { return maxWaves; }
    int GetTurnCounter() const { return turnCounter; }

    const std::vector<CombatLogEntry>& GetCombatLog() const { return combatLog; }
    void AddCombatLog(const std::string& text, ColorRGBA color = { 255, 255, 255, 255 });
    void AddCombatLog(const std::string& text, Color color) {
        AddCombatLog(text, ColorRGBA{ color.r, color.g, color.b, color.a });
    }

    void CheckBattleEndConditions();
};
