#include "Entities/Skill.hpp"
#include <algorithm>

Skill::Skill(const std::string& skillId, const std::string& skillName, const std::string& desc,
             Element prim, Element sec, int dmg, int shield, int cooldown, TargetType target, ColorRGBA color)
    : id(skillId), name(skillName), description(desc), baseElement(prim),
      secondaryElement(sec), baseDamage(dmg), baseShield(shield), maxCooldown(cooldown),
      currentCooldown(0), targetType(target), themeColor(color) {
}

Element Skill::GetFinalElement() const {
    Element elem = baseElement;
    for (const auto& rune : socketedRunes) {
        if (rune.overrideElement != Element::NONE) {
            elem = rune.overrideElement;
        }
    }
    return elem;
}

Element Skill::GetFinalSecondaryElement() const {
    Element sec = secondaryElement;
    for (const auto& rune : socketedRunes) {
        if (rune.addSecondaryElement != Element::NONE) {
            sec = rune.addSecondaryElement;
        }
    }
    return sec;
}

int Skill::GetFinalDamage(int playerShield) const {
    float dmg = static_cast<float>(baseDamage);
    int flatBonus = 0;
    float mult = 1.0f;
    int shieldBonus = 0;

    for (const auto& rune : socketedRunes) {
        flatBonus += rune.bonusDamage;
        mult *= rune.damageMultiplier;
        if (rune.shieldScalingDamage && playerShield > 0) {
            shieldBonus += playerShield;
        }
    }

    int totalDmg = static_cast<int>((dmg + flatBonus) * mult) + shieldBonus;
    return std::max(0, totalDmg);
}

int Skill::GetFinalShield() const {
    int shield = baseShield;
    for (const auto& rune : socketedRunes) {
        shield += rune.bonusShield;
    }
    return std::max(0, shield);
}

int Skill::GetFinalCooldown() const {
    int cd = maxCooldown;
    for (const auto& rune : socketedRunes) {
        cd += rune.cooldownDelta;
    }
    return std::max(0, cd);
}

ColorRGBA Skill::GetEffectiveThemeColor() const {
    Element finalElem = GetFinalElement();
    if (finalElem != baseElement && finalElem != Element::NONE) {
        return GetElementColorRGBA(finalElem);
    }
    return themeColor;
}

bool Skill::HasChainAoE() const {
    for (const auto& rune : socketedRunes) {
        if (rune.chainAoEOnHit) return true;
    }
    return false;
}

bool Skill::HasFreezeWet() const {
    for (const auto& rune : socketedRunes) {
        if (rune.freezeWetTarget) return true;
    }
    return false;
}

bool Skill::HasShieldScaling() const {
    for (const auto& rune : socketedRunes) {
        if (rune.shieldScalingDamage) return true;
    }
    return false;
}

bool Skill::AttachRune(const Rune& rune) {
    if (socketedRunes.size() < MAX_RUNE_SLOTS) {
        socketedRunes.push_back(rune);
        return true;
    } else {
        // If slots are full, replace the second rune slot
        socketedRunes.back() = rune;
        return true;
    }
}

void Skill::RemoveRune(const std::string& runeId) {
    socketedRunes.erase(
        std::remove_if(socketedRunes.begin(), socketedRunes.end(),
                       [&runeId](const Rune& r) { return r.id == runeId; }),
        socketedRunes.end()
    );
}

void Skill::ClearRunes() {
    socketedRunes.clear();
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
