#include "Entities/Skill.hpp"

Skill::Skill(const std::string& skillId, const std::string& skillName, const std::string& desc,
             Element prim, Element sec, int dmg, int cooldown, TargetType target, Color color)
    : id(skillId), name(skillName), description(desc), primaryElement(prim),
      secondaryElement(sec), baseDamage(dmg), maxCooldown(cooldown), currentCooldown(0),
      targetType(target), themeColor(color) {
}

SkillSystem::SkillSystem() {
    InitializeDefaultSkills();
}

void SkillSystem::InitializeDefaultSkills() {
    skills.clear();

    // 1. Torrent Slash (Water primer - low CD)
    skills.emplace_back(
        "skill_water",
        "Torrent Slash",
        "Deals 14 Water DMG. Inflicts [WET] (2T).\nSynergy: Combines with Elec/Cold.",
        Element::WET,
        Element::NONE,
        14,
        0, // 0 CD - Basic attack primer
        TargetType::SINGLE_ENEMY,
        (Color){ 52, 152, 219, 255 }
    );

    // 2. Ignition Flask (Oil + Fire combo trigger)
    skills.emplace_back(
        "skill_fire",
        "Ignition Flask",
        "Deals 10 Fire DMG. Drenches in [OIL] (2T) and ignites for immediate [EXPLOSION].",
        Element::FIRE,
        Element::OIL,
        10,
        2, // 2 CD
        TargetType::SINGLE_ENEMY,
        (Color){ 231, 76, 60, 255 }
    );

    // 3. Thunder Strike (Chain lightning trigger on Wet)
    skills.emplace_back(
        "skill_lightning",
        "Thunder Strike",
        "Deals 24 Lightning DMG. Triggers [SHOCK] AoE chain arcs to all enemies on [WET] targets.",
        Element::LIGHTNING,
        Element::NONE,
        24,
        2, // 2 CD
        TargetType::SINGLE_ENEMY,
        (Color){ 241, 196, 15, 255 }
    );

    // 4. Glacial Lance (Freeze trigger on Wet)
    skills.emplace_back(
        "skill_ice",
        "Glacial Lance",
        "Deals 18 Cold DMG. Inflicts [COLD]. If target is [WET], flash-freezes and skips their turn.",
        Element::COLD,
        Element::NONE,
        18,
        3, // 3 CD
        TargetType::SINGLE_ENEMY,
        (Color){ 162, 222, 255, 255 }
    );
}

void SkillSystem::TickCooldowns() {
    for (auto& skill : skills) {
        skill.UpdateCooldown();
    }
}

void SkillSystem::ResetCooldowns() {
    for (auto& skill : skills) {
        skill.ResetCooldown();
    }
}

Skill* SkillSystem::GetSkill(int index) {
    if (index >= 0 && index < static_cast<int>(skills.size())) {
        return &skills[index];
    }
    return nullptr;
}

const Skill* SkillSystem::GetSkill(int index) const {
    if (index >= 0 && index < static_cast<int>(skills.size())) {
        return &skills[index];
    }
    return nullptr;
}

bool SkillSystem::CanUseSkill(int index) const {
    const Skill* s = GetSkill(index);
    return s && s->IsReady();
}

void SkillSystem::UseSkill(int index) {
    Skill* s = GetSkill(index);
    if (s) {
        s->TriggerCooldown();
    }
}
