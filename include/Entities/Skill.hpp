#pragma once

#include "Core/Types.hpp"
#include <vector>
#include <string>

// ============================================================================
// Skill Mutation Rune (Extensible modifier for Phase 3 Mutation Runes)
// ============================================================================
struct SkillRune {
    std::string id;
    std::string name;
    std::string description;
    Element overrideElement = Element::NONE;     // If not NONE, mutates primary element
    Element addSecondaryElement = Element::NONE; // Additional element tag
    int bonusDamage = 0;
    float damageMultiplier = 1.0f;
    int bonusShield = 0;
    int cooldownDelta = 0;

    SkillRune(const std::string& runeId = "", const std::string& runeName = "",
              const std::string& desc = "", Element elem = Element::NONE, int bonusDmg = 0)
        : id(runeId), name(runeName), description(desc), overrideElement(elem), bonusDamage(bonusDmg) {}
};

// ============================================================================
// Skill Class (Pure C++17, Rune Mutation Support)
// ============================================================================
class Skill {
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

    // Attached Mutation Runes
    std::vector<SkillRune> attachedRunes;

public:
    Skill(const std::string& skillId = "", const std::string& skillName = "", const std::string& desc = "",
          Element prim = Element::NONE, Element sec = Element::NONE, int dmg = 10, int shield = 0,
          int cooldown = 0, TargetType target = TargetType::SINGLE_ENEMY,
          ColorRGBA color = { 255, 255, 255, 255 });

    // Dynamic Effective Value Queries (Calculated with Runes)
    Element GetEffectiveElement() const;
    Element GetEffectiveSecondaryElement() const;
    int GetEffectiveDamage() const;
    int GetEffectiveShield() const;
    int GetEffectiveMaxCooldown() const;

    // Rune Socketing Management
    void AttachRune(const SkillRune& rune);
    void RemoveRune(const std::string& runeId);
    void ClearRunes();
    const std::vector<SkillRune>& GetAttachedRunes() const { return attachedRunes; }

    // Cooldown Management
    bool IsReady() const { return currentCooldown <= 0; }
    void TriggerCooldown() { currentCooldown = GetEffectiveMaxCooldown(); }
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
