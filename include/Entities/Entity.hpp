#pragma once

#include "Core/Types.hpp"
#include "Systems/ElementalSystem.hpp"

class Entity {
protected:
    std::string name;
    int hp;
    int maxHp;
    int shield;
    std::vector<StatusInstance> statusBuffer;
    bool isFrozen;
    Vector2 position;

    // Visual state data
    float hitShakeTimer;
    float flashTimer;

public:
    Entity(const std::string& entityName = "Entity", int maxHealth = 100);
    virtual ~Entity() = default;

    virtual DamageReport ApplyIncomingDamage(int incomingDamage, Element incomingElement = Element::NONE,
                                            StanceType stance = StanceType::ATTACK, float weatherModifier = 1.0f);
    void InflictElement(Element elem, int duration = 2);
    void TickStatusEffects(std::vector<std::string>& outLogs);
    void ClearElement(Element elem);
    bool HasElement(Element elem) const;

    void AddShield(int amount) { shield += amount; }
    void ResetShield() { shield = 0; }
    void Heal(int amount);

    bool IsAlive() const { return hp > 0; }
    bool IsFrozen() const { return isFrozen; }
    void SetFrozen(bool frozen) { isFrozen = frozen; }

    const std::string& GetName() const { return name; }
    int GetHp() const { return hp; }
    int GetMaxHp() const { return maxHp; }
    int GetShield() const { return shield; }
    const std::vector<StatusInstance>& GetStatusBuffer() const { return statusBuffer; }
    Vector2 GetPosition() const { return position; }
    void SetPosition(Vector2 pos) { position = pos; }

    void UpdateVisuals(float dt);
    float GetFlashAlpha() const { return flashTimer > 0.0f ? flashTimer / 0.2f : 0.0f; }
    Vector2 GetRenderOffset() const;
};
