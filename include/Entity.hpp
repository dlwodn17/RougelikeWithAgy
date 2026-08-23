#pragma once

#include "Common.hpp"
#include "ElementalSystem.hpp"

struct DamageReport {
    int rawDamage = 0;
    int mitigatedDamage = 0;
    int shieldAbsorbed = 0;
    int healthDamage = 0;
    bool wasParried = false;
    bool wasShielded = false;
    bool wasCritical = false;
    bool causedDeath = false;
    ReactionOutcome reaction;
};

class Entity {
protected:
    std::string name;
    int hp;
    int maxHp;
    int shield;
    std::vector<StatusInstance> statusBuffer;
    bool isFrozen;
    bool isStunned;
    Vector2 visualPos;
    Vector2 targetPos;
    Color primaryColor;
    float hitShakeTimer;
    float flashTimer;

public:
    Entity(std::string name, int maxHp, Color color = WHITE);
    virtual ~Entity() = default;

    // Getters & Setters
    const std::string& GetName() const { return name; }
    int GetHp() const { return hp; }
    int GetMaxHp() const { return maxHp; }
    int GetShield() const { return shield; }
    bool IsAlive() const { return hp > 0; }
    bool IsFrozen() const { return isFrozen; }
    void SetFrozen(bool frozen) { isFrozen = frozen; }
    bool IsStunned() const { return isStunned; }
    void SetStunned(bool stunned) { isStunned = stunned; }
    
    std::vector<StatusInstance>& GetStatusBuffer() { return statusBuffer; }
    const std::vector<StatusInstance>& GetStatusBuffer() const { return statusBuffer; }

    Vector2 GetPosition() const { return visualPos; }
    void SetPosition(Vector2 pos) { visualPos = pos; targetPos = pos; }
    Color GetColor() const { return primaryColor; }

    // Combat Methods
    virtual DamageReport ApplyIncomingDamage(
        int rawDamage, 
        Element element, 
        StanceType receiverStance = StanceType::ATTACK,
        float weatherMultiplier = 1.0f
    );

    void AddShield(int amount);
    void ResetShield();
    void Heal(int amount);

    // Apply elemental status
    void InflictElement(Element elem, int duration = 2, int stacks = 1);
    bool HasElement(Element elem) const;
    void ClearElement(Element elem);

    // Status ticks at end of turn
    virtual void TickStatusEffects(std::vector<std::string>& logs);

    // Visual updates
    void TriggerHitVisuals();
    void UpdateVisuals(float dt);
    float GetFlashAlpha() const { return flashTimer > 0.0f ? flashTimer / 0.2f : 0.0f; }
    Vector2 GetRenderOffset() const;
};

class Player : public Entity {
private:
    StanceType currentStance;
    int comboScore;

public:
    Player(std::string name = "Arcane Duelist", int maxHp = 100);

    StanceType GetStance() const { return currentStance; }
    void SetStance(StanceType stance) { currentStance = stance; }

    int GetComboScore() const { return comboScore; }
    void AddComboScore(int points) { comboScore += points; }

    DamageReport ApplyIncomingDamage(
        int rawDamage, 
        Element element, 
        StanceType receiverStance = StanceType::ATTACK,
        float weatherMultiplier = 1.0f
    ) override;
};

class Enemy : public Entity {
private:
    std::string enemyType;
    Intent currentIntent;
    int patternIndex;
    int tier;

public:
    Enemy(std::string name, std::string type, int maxHp, Color color, int tier = 1);

    const std::string& GetEnemyType() const { return enemyType; }
    const Intent& GetIntent() const { return currentIntent; }
    int GetTier() const { return tier; }

    void DecideIntent(int turnNumber, const Player& player, WeatherType currentWeather);
    void AdvancePattern() { patternIndex++; }
};
