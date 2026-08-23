#pragma once

#include "Core/Types.hpp"
#include "Core/Localization.hpp"
#include <string>
#include <vector>

// ============================================================================
// Rune Modifier Types
// ============================================================================
enum class RuneModifierType {
    ELEMENT_CONVERSION = 0, // 속성 변환 (예: FIRE -> COLD)
    STAT_MUTATION,          // 수치 개조 (데미지 배율, 쿨다운 증감)
    STATUS_INFUSION,        // 상태이상 주입 (적중 시 추가 원소 상태 부여)
    STANCE_SYNERGY,         // 스탠스 연계 (스탠스별 보너스)
    SPECIAL_TRIGGER         // 특수 트리거 (광역 감전, 조건부 빙결)
};

// ============================================================================
// Mutation Rune Structure
// ============================================================================
struct Rune {
    std::string id;
    std::string name;           // English Name
    std::string nameKo;         // Korean Name
    std::string description;    // English Description
    std::string descriptionKo;  // Korean Description
    RuneModifierType modifierType = RuneModifierType::STAT_MUTATION;

    // 1. Element Modification
    Element overrideElement = Element::NONE;     // If not NONE, mutates skill's primary element
    Element addSecondaryElement = Element::NONE; // Additional element inflicted on hit (e.g., OIL)

    // 2. Stat Modification
    int bonusDamage = 0;
    float damageMultiplier = 1.0f;
    int bonusShield = 0;
    int cooldownDelta = 0;

    // 3. Special Triggers & Mechanics
    bool freezeWetTarget = false;      // If target is WET, immediately flash-freeze
    bool chainAoEOnHit = false;        // Hits all alive enemies with shockwave arcs
    bool shieldScalingDamage = false;  // Adds current DEF shield amount as bonus damage

    // 4. Visuals
    ColorRGBA runeColor = { 241, 196, 15, 255 };

    Rune() = default;

    Rune(const std::string& rId, const std::string& rName, const std::string& rNameKo,
         const std::string& desc, const std::string& descKo, RuneModifierType modType,
         ColorRGBA col = { 241, 196, 15, 255 })
        : id(rId), name(rName), nameKo(rNameKo), description(desc), descriptionKo(descKo),
          modifierType(modType), runeColor(col) {}

    const std::string& GetName() const {
        return Localization::IsKorean() ? nameKo : name;
    }

    const std::string& GetDescription() const {
        return Localization::IsKorean() ? descriptionKo : description;
    }

    const char* GetCategoryName() const {
        if (Localization::IsKorean()) {
            switch (modifierType) {
                case RuneModifierType::ELEMENT_CONVERSION: return "속성 변환";
                case RuneModifierType::STAT_MUTATION:      return "수치 개조";
                case RuneModifierType::STATUS_INFUSION:    return "상태이상 주입";
                case RuneModifierType::STANCE_SYNERGY:     return "스탠스 연계";
                case RuneModifierType::SPECIAL_TRIGGER:    return "특수 반응";
            }
        } else {
            switch (modifierType) {
                case RuneModifierType::ELEMENT_CONVERSION: return "Element Convert";
                case RuneModifierType::STAT_MUTATION:      return "Stat Mutation";
                case RuneModifierType::STATUS_INFUSION:    return "Status Infusion";
                case RuneModifierType::STANCE_SYNERGY:     return "Stance Synergy";
                case RuneModifierType::SPECIAL_TRIGGER:    return "Special Trigger";
            }
        }
        return "Rune";
    }
};

// Aliases for compatibility
using SkillRune = Rune;
