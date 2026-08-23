#include "Entities/Skill.hpp"

Skill::Skill(const std::string& skillId, const std::string& skillName, const std::string& desc,
             Element prim, Element sec, int dmg, int shield, int cooldown, TargetType target, ColorRGBA color)
    : id(skillId), name(skillName), description(desc), baseElement(prim),
      secondaryElement(sec), baseDamage(dmg), baseShield(shield), maxCooldown(cooldown),
      currentCooldown(0), targetType(target), themeColor(color) {
}

Element Skill::GetEffectiveElement() const {
    Element elem = baseElement;
    for (const auto& rune : attachedRunes) {
        if (rune.overrideElement != Element::NONE) {
            elem = rune.overrideElement;
        }
    }
    return elem;
}

Element Skill::GetEffectiveSecondaryElement() const {
    Element sec = secondaryElement;
    for (const auto& rune : attachedRunes) {
        if (rune.addSecondaryElement != Element::NONE) {
            sec = rune.addSecondaryElement;
        }
    }
    return sec;
}

int Skill::GetEffectiveDamage() const {
    float dmg = static_cast<float>(baseDamage);
    int flatBonus = 0;
    float mult = 1.0f;

    for (const auto& rune : attachedRunes) {
        flatBonus += rune.bonusDamage;
        mult *= rune.damageMultiplier;
    }

    return std::max(0, static_cast<int>((dmg + flatBonus) * mult));
}

int Skill::GetEffectiveShield() const {
    int shield = baseShield;
    for (const auto& rune : attachedRunes) {
        shield += rune.bonusShield;
    }
    return std::max(0, shield);
}

int Skill::GetEffectiveMaxCooldown() const {
    int cd = maxCooldown;
    for (const auto& rune : attachedRunes) {
        cd += rune.cooldownDelta;
    }
    return std::max(0, cd);
}

void Skill::AttachRune(const SkillRune& rune) {
    attachedRunes.push_back(rune);
}

void Skill::RemoveRune(const std::string& runeId) {
    attachedRunes.erase(
        std::remove_if(attachedRunes.begin(), attachedRunes.end(),
                       [&runeId](const SkillRune& r) { return r.id == runeId; }),
        attachedRunes.end()
    );
}

void Skill::ClearRunes() {
    attachedRunes.clear();
}

SkillSystem::SkillSystem() {
    InitializeDefaultSkills();
}

void SkillSystem::InitializeDefaultSkills() {
    skills.clear();

    // 1. Torrent Slash (Water primer - 0 CD basic attack)
    skills.emplace_back(
        "skill_water",
        "Torrent Slash",
        "Deals 14 Water DMG. Inflicts [WET] (2T).\nSynergy: Combines with Elec/Cold.",
        Element::WET,
        Element::NONE,
        14,
        0, // Shield
        0, // 0 CD
        TargetType::SINGLE_ENEMY,
        ColorRGBA{ 52, 152, 219, 255 }
    );

    // 2. Ignition Flask (Oil + Fire combo trigger)
    skills.emplace_back(
        "skill_fire",
        "Ignition Flask",
        "Deals 10 Fire DMG. Drenches target in [OIL] (2T) and ignites for [EXPLOSION].",
        Element::FIRE,
        Element::OIL,
        10,
        0,
        2, // 2 CD
        TargetType::SINGLE_ENEMY,
        ColorRGBA{ 231, 76, 60, 255 }
    );

    // 3. Thunder Strike (Shock AoE chain trigger on Wet)
    skills.emplace_back(
        "skill_lightning",
        "Thunder Strike",
        "Deals 24 Lightning DMG. Triggers [SHOCK] AoE chain arcs on [WET] targets.",
        Element::LIGHTNING,
        Element::NONE,
        24,
        0,
        2, // 2 CD
        TargetType::SINGLE_ENEMY,
        ColorRGBA{ 241, 196, 15, 255 }
    );

    // 4. Glacial Lance (Frozen stun trigger on Wet)
    skills.emplace_back(
        "skill_ice",
        "Glacial Lance",
        "Deals 18 Cold DMG. Inflicts [COLD]. If target is [WET], flash-freezes and skips their turn.",
        Element::COLD,
        Element::NONE,
        18,
        0,
        3, // 3 CD
        TargetType::SINGLE_ENEMY,
        ColorRGBA{ 162, 222, 255, 255 }
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
