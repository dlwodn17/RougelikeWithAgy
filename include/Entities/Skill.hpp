#pragma once

#include "Core/Types.hpp"
#include "Entities/Rune.hpp"
#include <vector>
#include <string>
#include <algorithm>

// ============================================================================
// Skill Class (Pure C++17, Rune Socketing & Dynamic Mutation Support)
// ============================================================================
class Skill {
public:
    static constexpr size_t MAX_RUNE_SLOTS = 2;

private:
    std::string id;
    std::string name;
    std::string description;
    Element baseElement;
    Element secondaryElement;
    int baseDamage;
    int baseShield;
    int maxCooldown;
    int currentCooldown;
    TargetType targetType;
    ColorRGBA themeColor;

    // Attached Mutation Runes (Max 2 Slots)
    std::vector<Rune> socketedRunes;

public:
    Skill(const std::string& skillId = "", const std::string& skillName = "", const std::string& desc = "",
          Element prim = Element::NONE, Element sec = Element::NONE, int dmg = 10, int shield = 0,
          int cooldown = 0, TargetType target = TargetType::SINGLE_ENEMY,
          ColorRGBA color = { 255, 255, 255, 255 });

    // Dynamic Effective Value Queries (Calculated with Runes)
    Element GetFinalElement() const;
    Element GetEffectiveElement() const { return GetFinalElement(); }

    Element GetFinalSecondaryElement() const;
    Element GetEffectiveSecondaryElement() const { return GetFinalSecondaryElement(); }

    int GetFinalDamage(int playerShield = 0) const;
    int GetEffectiveDamage(int playerShield = 0) const { return GetFinalDamage(playerShield); }

    int GetFinalShield() const;
    int GetEffectiveShield() const { return GetFinalShield(); }

    int GetFinalCooldown() const;
    int GetEffectiveMaxCooldown() const { return GetFinalCooldown(); }

    ColorRGBA GetEffectiveThemeColor() const;

    // Rune Special Mechanics Queries
    bool HasChainAoE() const;
    bool HasFreezeWet() const;
    bool HasShieldScaling() const;

    // Rune Socketing Management
    bool CanSocketRune() const { return socketedRunes.size() < MAX_RUNE_SLOTS; }
    bool AttachRune(const Rune& rune);
    void RemoveRune(const std::string& runeId);
    void ClearRunes();
    const std::vector<Rune>& GetSocketedRunes() const { return socketedRunes; }
    const std::vector<Rune>& GetAttachedRunes() const { return socketedRunes; }

    // Cooldown Management
    bool IsReady() const { return currentCooldown <= 0; }
    void TriggerCooldown() { currentCooldown = GetFinalCooldown(); }
    void UpdateCooldown() { if (currentCooldown > 0) currentCooldown--; }
    void ResetCooldown() { currentCooldown = 0; }

    // Getters & Setters
    const std::string& GetId() const { return id; }
    const std::string& GetName() const { return name; }
    const std::string& GetDescription() const { return description; }
    Element GetBaseElement() const { return baseElement; }
    Element GetSecondaryElement() const { return secondaryElement; }
    int GetBaseDamage() const { return baseDamage; }
    int GetBaseShield() const { return baseShield; }
    int GetCurrentCooldown() const { return currentCooldown; }
    TargetType GetTargetType() const { return targetType; }
    ColorRGBA GetThemeColor() const { return themeColor; }
    void SetThemeColor(ColorRGBA col) { themeColor = col; }
};

// ============================================================================
// Skill System (Deck & Cooldown Manager)
// ============================================================================
class SkillSystem {
private:
    std::vector<Skill> skills;

public:
    SkillSystem();

    void InitializeDefaultSkills();
    void TickCooldowns();
    void ResetCooldowns();

    std::vector<Skill>& GetSkills() { return skills; }
    const std::vector<Skill>& GetSkills() const { return skills; }

    Skill* GetSkill(int index);
    const Skill* GetSkill(int index) const;

    bool CanUseSkill(int index) const;
    void UseSkill(int index);
};
