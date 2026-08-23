#pragma once

#include "Systems/RewardSystem.hpp"
#include "Entities/Skill.hpp"
#include "Core/Constants.hpp"

// ============================================================================
// Reward Renderer (Mutation Rune Draft & Skill Socketing Overlay)
// ============================================================================
class RewardRenderer {
public:
    static void DrawRewardScreen(const RewardSystem& rewardSystem, const SkillSystem& skillSystem);

private:
    static void DrawRuneCard(Rectangle rec, const Rune& rune, int runeIndex, bool isHovered, bool isSelected);
    static void DrawSocketingModal(const RewardSystem& rewardSystem, const SkillSystem& skillSystem);
};
