#pragma once

#include "Entities/Entity.hpp"
#include "Entities/Skill.hpp"

class Player : public Entity {
private:
    SkillSystem skillSystem;

public:
    Player(const std::string& playerName = "Arcane Duelist", int maxHealth = 100);

    SkillSystem& GetSkillSystem() { return skillSystem; }
    const SkillSystem& GetSkillSystem() const { return skillSystem; }

    std::vector<Skill>& GetSkills() { return skillSystem.GetSkills(); }
    const std::vector<Skill>& GetSkills() const { return skillSystem.GetSkills(); }

    Skill* GetSkill(int index) { return skillSystem.GetSkill(index); }
    const Skill* GetSkill(int index) const { return skillSystem.GetSkill(index); }

    bool CanUseSkill(int index) const { return skillSystem.CanUseSkill(index); }
    void UseSkill(int index) { skillSystem.UseSkill(index); }
    void TickCooldowns() { skillSystem.TickCooldowns(); }
    void ResetCooldowns() { skillSystem.ResetCooldowns(); }
};
