#include "SkillSystem.hpp"

SkillSystem::SkillSystem() {
    InitializeDefaultSkills();
}

void SkillSystem::InitializeDefaultSkills() {
    playerSkills.clear();

    // Skill 1: Torrent Slash (Cooldown: 0) - Water primer
    Skill skill1;
    skill1.id = 0;
    skill1.name = "Torrent Slash";
    skill1.description = "Strikes for 14 Water damage and applies [WET]. Essential primer.";
    skill1.baseDamage = 14;
    skill1.primaryElement = Element::WET;
    skill1.secondaryElement = Element::NONE;
    skill1.targetType = TargetType::SINGLE_ENEMY;
    skill1.maxCooldown = 0;
    skill1.currentCooldown = 0;
    skill1.shieldGain = 0;
    skill1.themeColor = (Color){ 52, 152, 219, 255 };
    playerSkills.push_back(skill1);

    // Skill 2: Ignition Flask (Cooldown: 2) - Oil + Fire setup
    Skill skill2;
    skill2.id = 1;
    skill2.name = "Ignition Flask";
    skill2.description = "Hurls explosive flask dealing 10 Fire damage. Applies [OIL] & triggers [EXPLOSION].";
    skill2.baseDamage = 10;
    skill2.primaryElement = Element::OIL;
    skill2.secondaryElement = Element::FIRE;
    skill2.targetType = TargetType::SINGLE_ENEMY;
    skill2.maxCooldown = 2;
    skill2.currentCooldown = 0;
    skill2.shieldGain = 0;
    skill2.themeColor = (Color){ 230, 126, 34, 255 };
    playerSkills.push_back(skill2);

    // Skill 3: Thunder Strike (Cooldown: 2) - Lightning burst & AoE chain
    Skill skill3;
    skill3.id = 2;
    skill3.name = "Thunder Strike";
    skill3.description = "Calls down 24 Lightning damage. Triggers [SHOCK] AoE arc on [WET] targets!";
    skill3.baseDamage = 24;
    skill3.primaryElement = Element::LIGHTNING;
    skill3.secondaryElement = Element::NONE;
    skill3.targetType = TargetType::SINGLE_ENEMY;
    skill3.maxCooldown = 2;
    skill3.currentCooldown = 0;
    skill3.shieldGain = 0;
    skill3.themeColor = (Color){ 241, 196, 15, 255 };
    playerSkills.push_back(skill3);

    // Skill 4: Glacial Lance (Cooldown: 3) - Cold damage & Freeze
    Skill skill4;
    skill4.id = 3;
    skill4.name = "Glacial Lance";
    skill4.description = "Impales for 18 Cold damage. Triggers [FROZEN] on [WET] targets (skips turn).";
    skill4.baseDamage = 18;
    skill4.primaryElement = Element::COLD;
    skill4.secondaryElement = Element::NONE;
    skill4.targetType = TargetType::SINGLE_ENEMY;
    skill4.maxCooldown = 3;
    skill4.currentCooldown = 0;
    skill4.shieldGain = 0;
    skill4.themeColor = (Color){ 162, 222, 255, 255 };
    playerSkills.push_back(skill4);
}

Skill* SkillSystem::GetSkill(int index) {
    if (index >= 0 && index < static_cast<int>(playerSkills.size())) {
        return &playerSkills[index];
    }
    return nullptr;
}

bool SkillSystem::CanUseSkill(int index) const {
    if (index >= 0 && index < static_cast<int>(playerSkills.size())) {
        return playerSkills[index].IsReady();
    }
    return false;
}

void SkillSystem::UseSkill(int index) {
    if (index >= 0 && index < static_cast<int>(playerSkills.size())) {
        playerSkills[index].TriggerCooldown();
    }
}

void SkillSystem::UpdateCooldowns() {
    for (auto& skill : playerSkills) {
        skill.UpdateCooldown();
    }
}

void SkillSystem::TickCooldowns() {
    UpdateCooldowns();
}

void SkillSystem::TickAllCooldowns() {
    UpdateCooldowns();
}

void SkillSystem::ResetAllCooldowns() {
    for (auto& skill : playerSkills) {
        skill.ResetCooldown();
    }
}

