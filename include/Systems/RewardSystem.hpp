#pragma once

#include "Entities/Rune.hpp"
#include "Entities/Skill.hpp"
#include <vector>
#include <random>

// ============================================================================
// Reward System (Wave Victory Mutation Rune Draft & Socketing Manager)
// ============================================================================
class RewardSystem {
private:
    std::vector<Rune> runePool;
    std::vector<Rune> offeredRunes;
    int selectedRuneIndex;
    bool socketingPopupOpen;
    std::mt19937 rng;

public:
    RewardSystem();

    void InitializeRunePool();
    void GenerateRewardRunes(int count = 3);

    // Draft & Selection
    void SelectRune(int index);
    void CancelSelection();
    bool SocketRuneToSkill(SkillSystem& skillSystem, int skillIndex);

    // Queries
    const std::vector<Rune>& GetOfferedRunes() const { return offeredRunes; }
    const Rune* GetSelectedRune() const;
    int GetSelectedRuneIndex() const { return selectedRuneIndex; }
    bool IsSocketingPopupOpen() const { return socketingPopupOpen; }

    void Reset();
};
