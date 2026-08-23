#pragma once

#include "Core/Types.hpp"

class Skill {
public:
    std::string id;
    std::string name;
    std::string description;
    Element primaryElement;
    Element secondaryElement;
    int baseDamage;
    int maxCooldown;
    int currentCooldown;
    TargetType targetType;
    Color themeColor;

    Skill(const std::string& skillId, const std::string& skillName, const std::string& desc,
          Element prim, Element sec, int dmg, int cooldown, TargetType target, Color color);

    bool IsReady() const { return currentCooldown <= 0; }
    void TriggerCooldown() { currentCooldown = maxCooldown; }
    void UpdateCooldown() {
        if (currentCooldown > 0) currentCooldown--;
    }
    void ResetCooldown() { currentCooldown = 0; }
};

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
