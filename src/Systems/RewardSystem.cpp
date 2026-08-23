#include "Systems/RewardSystem.hpp"
#include <chrono>
#include <algorithm>

RewardSystem::RewardSystem()
    : selectedRuneIndex(-1), socketingPopupOpen(false),
      rng(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count())) {
    InitializeRunePool();
}

void RewardSystem::InitializeRunePool() {
    runePool.clear();

    // 1. 서리불꽃 룬 (Frostfire)
    {
        Rune r("rune_frostfire", "Frostfire Rune", "서리불꽃 룬",
               "Mutates element to COLD (+6 DMG). If target is WET, flash-freezes them instantly.",
               "스킬 속성을 [냉기]로 변환(+6 피해). 대상이 [수분] 상태면 즉시 빙결시킵니다.",
               RuneModifierType::ELEMENT_CONVERSION,
               ColorRGBA{ 162, 222, 255, 255 });
        r.overrideElement = Element::COLD;
        r.bonusDamage = 6;
        r.freezeWetTarget = true;
        runePool.push_back(r);
    }

    // 2. 연쇄 전도체 (Chain Conductor)
    {
        Rune r("rune_chain_conductor", "Chain Conductor", "연쇄 전도체",
               "Mutates element to LIGHTNING (-20% DMG). Arcs shockwave to all alive enemies.",
               "스킬 속성을 [전기]로 변환(-20% 피해). 타격 시 모든 살아있는 적에게 감전 아크를 전파합니다.",
               RuneModifierType::SPECIAL_TRIGGER,
               ColorRGBA{ 241, 196, 15, 255 });
        r.overrideElement = Element::LIGHTNING;
        r.damageMultiplier = 0.80f;
        r.chainAoEOnHit = true;
        runePool.push_back(r);
    }

    // 3. 농축 기폭제 (Volatile Core)
    {
        Rune r("rune_volatile_core", "Volatile Core", "농축 기폭제",
               "Increases damage by +50%, but increases skill cooldown by +1 Turn.",
               "스킬 피해량이 +50% 대폭 증가하지만, 쿨다운이 1턴 증가합니다.",
               RuneModifierType::STAT_MUTATION,
               ColorRGBA{ 231, 76, 60, 255 });
        r.damageMultiplier = 1.50f;
        r.cooldownDelta = 1;
        runePool.push_back(r);
    }

    // 4. 산성 주입 (Acid Infusion)
    {
        Rune r("rune_acid_infusion", "Acid Infusion", "산성 주입",
               "Infuses skill with combustible [OIL] (+4 DMG) to prepare devastating Explosions.",
               "스킬에 인화성 [기름] 속성을 추가 주입(+4 피해)하여 강력한 폭발 연계를 준비합니다.",
               RuneModifierType::STATUS_INFUSION,
               ColorRGBA{ 108, 92, 231, 255 });
        r.addSecondaryElement = Element::OIL;
        r.bonusDamage = 4;
        runePool.push_back(r);
    }

    // 5. 급속 냉각기 (Quick Cycle)
    {
        Rune r("rune_quick_cycle", "Quick Cycle", "급속 냉각기",
               "Reduces cooldown by -1 Turn (Min 0), with -15% damage reduction.",
               "스킬 쿨다운을 1턴 감소(최소 0턴)시킵니다. (피해량 -15% 보정)",
               RuneModifierType::STAT_MUTATION,
               ColorRGBA{ 46, 204, 113, 255 });
        r.cooldownDelta = -1;
        r.damageMultiplier = 0.85f;
        runePool.push_back(r);
    }

    // 6. 철벽의 일격 (Aegis Strike)
    {
        Rune r("rune_aegis_strike", "Aegis Strike", "철벽의 일격",
               "Adds your current Shield value as extra damage. Grants +8 Shield on cast.",
               "자신의 현재 방어막(Shield) 수치만큼 추가 피해를 입히며 +8 방어막을 부여합니다.",
               RuneModifierType::STANCE_SYNERGY,
               ColorRGBA{ 52, 152, 219, 255 });
        r.shieldScalingDamage = true;
        r.bonusShield = 8;
        runePool.push_back(r);
    }

    // 7. 과충전 코어 (Overcharged Core)
    {
        Rune r("rune_overcharged", "Overcharged Core", "과충전 코어",
               "Mutates element to LIGHTNING and grants flat +12 bonus damage.",
               "스킬을 [전기] 속성으로 변환하고 고정 +12 추가 피해를 부여합니다.",
               RuneModifierType::ELEMENT_CONVERSION,
               ColorRGBA{ 243, 156, 18, 255 });
        r.overrideElement = Element::LIGHTNING;
        r.bonusDamage = 12;
        runePool.push_back(r);
    }

    // 8. 초고온 기화 촉매 (Thermal Vaporizer)
    {
        Rune r("rune_thermal", "Thermal Vaporizer", "초고온 기화 촉매",
               "Mutates element to FIRE (+8 DMG) for explosive vapor and burn synergies.",
               "스킬을 [화염] 속성으로 변환(+8 피해)하여 증발/융해 및 폭발 반응을 극대화합니다.",
               RuneModifierType::ELEMENT_CONVERSION,
               ColorRGBA{ 230, 126, 34, 255 });
        r.overrideElement = Element::FIRE;
        r.bonusDamage = 8;
        runePool.push_back(r);
    }
}

void RewardSystem::GenerateRewardRunes(int count) {
    offeredRunes.clear();
    selectedRuneIndex = -1;
    socketingPopupOpen = false;

    if (runePool.empty()) InitializeRunePool();

    std::vector<size_t> indices(runePool.size());
    for (size_t i = 0; i < runePool.size(); ++i) indices[i] = i;

    std::shuffle(indices.begin(), indices.end(), rng);

    int pickCount = std::min(count, static_cast<int>(indices.size()));
    for (int i = 0; i < pickCount; ++i) {
        offeredRunes.push_back(runePool[indices[i]]);
    }
}

void RewardSystem::SelectRune(int index) {
    if (index >= 0 && index < static_cast<int>(offeredRunes.size())) {
        selectedRuneIndex = index;
        socketingPopupOpen = true;
    }
}

void RewardSystem::CancelSelection() {
    selectedRuneIndex = -1;
    socketingPopupOpen = false;
}

bool RewardSystem::SocketRuneToSkill(SkillSystem& skillSystem, int skillIndex) {
    if (selectedRuneIndex < 0 || selectedRuneIndex >= static_cast<int>(offeredRunes.size())) {
        return false;
    }

    Skill* targetSkill = skillSystem.GetSkill(skillIndex);
    if (!targetSkill) return false;

    const Rune& runeToAttach = offeredRunes[selectedRuneIndex];
    bool attached = targetSkill->AttachRune(runeToAttach);

    if (attached) {
        selectedRuneIndex = -1;
        socketingPopupOpen = false;
    }

    return attached;
}

const Rune* RewardSystem::GetSelectedRune() const {
    if (selectedRuneIndex >= 0 && selectedRuneIndex < static_cast<int>(offeredRunes.size())) {
        return &offeredRunes[selectedRuneIndex];
    }
    return nullptr;
}

void RewardSystem::Reset() {
    offeredRunes.clear();
    selectedRuneIndex = -1;
    socketingPopupOpen = false;
}
