#pragma once

#include "Common.hpp"

struct Skill {
    int id;
    std::string name;
    std::string description;
    int baseDamage;
    Element primaryElement;
    Element secondaryElement;
    TargetType targetType;
    int maxCooldown;
    int currentCooldown;
    int shieldGain;
    Color themeColor;

    bool IsReady() const { return currentCooldown <= 0; }
    void TriggerCooldown() { currentCooldown = maxCooldown; }
    void UpdateCooldown() { if (currentCooldown > 0) currentCooldown--; }
    void TickCooldown() { UpdateCooldown(); }
    void ResetCooldown() { currentCooldown = 0; }
};

class SkillSystem {
private:
    std::vector<Skill> playerSkills;

public:
    SkillSystem();

    void InitializeDefaultSkills();
    std::vector<Skill>& GetSkills() { return playerSkills; }
    const std::vector<Skill>& GetSkills() const { return playerSkills; }

    Skill* GetSkill(int index);
    bool CanUseSkill(int index) const;
    void UseSkill(int index);
    void UpdateCooldowns();
    void TickCooldowns();
    void TickAllCooldowns();
    void ResetAllCooldowns();
};
