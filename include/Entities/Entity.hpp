#pragma once

#include "Core/Types.hpp"
#include "Systems/ElementalSystem.hpp"
#include <unordered_map>
#include <vector>
#include <string>

// ============================================================================
// Entity Base Class (Pure C++17 Game Logic, Bitflag Statuses, No Draw Calls)
// ============================================================================
class Entity {
protected:
    std::string name;
    int hp;
    int maxHp;
    int shield;
    StanceType stance;

    // Bitflag Status Representation
    Element activeStatusMask;
    std::unordered_map<uint32_t, int> statusDurations; // Key: (uint32_t)Element, Value: duration

    bool isFrozen;
    float posX;
    float posY;

    // Visual timers queried by renderer
    float hitShakeTimer;
    float flashTimer;

public:
    Entity(const std::string& entityName = "Entity", int maxHealth = 100);
    virtual ~Entity() = default;

    // Core Damage & Elemental Reaction Processing
    virtual DamageReport ApplyIncomingDamage(int incomingDamage, Element incomingElement = Element::NONE,
                                            StanceType targetStance = StanceType::ATTACK, float weatherModifier = 1.0f);

    // Status / Bitmask Management
    void ApplyElement(Element elem, int duration = 2);
    void TickStatusEffects(std::vector<std::string>& outLogs);
    void ClearElement(Element elem);
    void ClearAllElements();
    bool HasElement(Element elem) const;
    Element GetActiveStatusMask() const { return activeStatusMask; }
    int GetElementDuration(Element elem) const;
    std::vector<StatusInstance> GetStatusInstances() const;

    // Stance & Vitals Management
    void SetStance(StanceType newStance) { stance = newStance; }
    StanceType GetStance() const { return stance; }

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

    // Coordinate & Visual Queries
    void SetPosition(float x, float y) { posX = x; posY = y; }
    void SetPosition(Vec2 pos) { posX = pos.x; posY = pos.y; }
    float GetX() const { return posX; }
    float GetY() const { return posY; }
    Vec2 GetPosition() const { return Vec2{ posX, posY }; }

    void UpdateVisuals(float dt);
    float GetFlashAlpha() const { return flashTimer > 0.0f ? flashTimer / 0.2f : 0.0f; }
    Vec2 GetRenderOffset() const;
    void GetRenderOffset(float& outX, float& outY) const;
};
